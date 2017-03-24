/*
 * This program is designed for raspberry pi 3
 *
 * After this version, parameter for
 * by Meng Cao
 * gcc -o btagent btagent_v0.8.4.c -lbluetooth
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>  //HCIGETDEVLIST
#include <sys/socket.h>
// Bluetooth
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
// Network
#include <netinet/in.h>
#include <netdb.h>
#include "blist.h"


#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

#define EIR_FLAGS                   0x01  /* flags */
#define EIR_UUID16_SOME             0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL              0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL              0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */

#define EIR_SERVICE_UUID128         0x15  /* service 128-bit UUID */

#define EIR_TX_POWER                0x0A  /* transmit power level */
#define EIR_DEVICE_ID               0x10  /* device ID */

#define MAX_ADDR_LEN                18    //inlcude '\0' char
#define MAX_MSG_SIZE                672 // Maximum message size

#define BT_AGENT_DEBUG

// Global Variables
int masterSock;
static volatile int signal_received = 0;
static struct hci_dev_info di;   //own bluetooth device information
FILE *g_logfd;
// TCP
int tcpSock;
char host[32] = {0};
char fileName[100] = "beacon.txt";
int millisecond = 5000;
int port = 9999;

// Configurables
int txEnable = 0;
int logEnable = 0;
int whiteList = 0;
int uuidEnable = 0;
int packetSize = 10;
int displayEnable = 0;


static inline double
microtime_calc (struct timeval *tv)
{
    return tv->tv_sec + (tv->tv_usec / 1000000.0);
}

static inline double
microtime (void)
{
    struct timeval tv;

    if (!gettimeofday (&tv, NULL))
        return microtime_calc (&tv);
    else
        return 0;
}

static int read_flags(uint8_t *flags, const uint8_t *data, size_t size)
{
    size_t offset;
    if (!flags || !data)
        return -EINVAL;
    offset = 0;
    while (offset < size) {
        uint8_t len = data[offset];
        uint8_t type;
        /* Check if it is the end of the significant part */
        if (len == 0)
            break;
        if (len + offset > size)
            break;
        type = data[offset + 1];

        if (type == FLAGS_AD_TYPE) {
            *flags = data[offset + 2];
            return 0;
        }
        offset += 1 + len;
    }
    return -ENOENT;
}

static int check_report_filter(uint8_t procedure, le_advertising_info *info)
{
    uint8_t flags;
    /* If no discovery procedure is set, all reports are treat as valid */
    if (procedure == 0)
        return 1;
    /* Read flags AD type value from the advertising report if it exists */
    if (read_flags(&flags, info->data, info->length))
        return 0;

    switch (procedure) {
        case 'l': /* Limited Discovery Procedure */
            if (flags & FLAGS_LIMITED_MODE_BIT)
                return 1;
            break;
        case 'g': /* General Discovery Procedure */
            if (flags & (FLAGS_LIMITED_MODE_BIT | FLAGS_GENERAL_MODE_BIT))
                return 1;
            break;
        default:
            fprintf(stderr, "Unknown discovery procedure\n");
    }
    return 0;
}

static void sigint_handler(int sig) {
    signal_received = sig;
    close(masterSock);
    close(tcpSock);
    printf("---------- ctrl-x ----------\n");
}

static void eir_parse_name(uint8_t *eir, size_t eir_len,
                        char *buf, size_t buf_len) {
    size_t offset;

    offset = 0;
    while (offset < eir_len) {
        uint8_t field_len = eir[0];
        size_t name_len;

        /* Check for the end of EIR */
        if (field_len == 0)
            break;

        if (offset + field_len > eir_len)
            goto failed;

        switch (eir[1]) {
        case EIR_NAME_SHORT:
        case EIR_NAME_COMPLETE:
            name_len = field_len - 1;
            if (name_len > buf_len)
                goto failed;

            memcpy(buf, &eir[2], name_len);
            return;
        }

        offset += field_len + 1;
        eir += field_len + 1;
    }

failed:
    snprintf(buf, buf_len, "(unknown)");
}


static int print_advertising_devices(int dd, uint8_t filter_type, char *loc)
{
    unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
    struct hci_filter nf, of;
    struct sigaction sa;
    socklen_t olen;
    int len;
    int idx = 0;
    char rssistr[15];
    char send_msg[MAX_MSG_SIZE] = { 0 };
    char txmessage[MAX_MSG_SIZE] = { 0 };
    struct sockaddr_l2 loc_addr = { 0 };
    // Add timestamp
    double current_time;
    char ctimestr[20];
    int count = 0;

    // tcp sock
    struct sockaddr_in serverAddr;
    struct hostent *remoteServer;
    char readBuffer[MAX_MSG_SIZE];

    list_t *lst = init_list(loc);

    // Open log file
    if ( logEnable == 1){
        g_logfd = fopen("log.txt", "w");
        if ( g_logfd == NULL ) {
            printf("open log file error!\n");
            exit(EXIT_FAILURE);
        }
    }

    olen = sizeof(of);
    if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
        printf("Could not get socket options\n");
        return -1;
    }

    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);

    if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
        printf("Could not set socket options\n");
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_NOCLDSTOP;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    /**************************************************************************/
    // Tcp connection
    if ( txEnable ) {
        //setup L2cap
        tcpSock = socket(AF_INET, SOCK_STREAM, 0);
        if (tcpSock < 0){
                perror("ERROR: Cannot open socket!");
                exit(0);
        }
        printf("INFO: Connecting host %s.\n", host);

        remoteServer = gethostbyname(host);
        if (remoteServer == NULL){
            fprintf(stderr, "ERROR: No such host at %s !\n", host);
            exit(0);
        }
        printf("INFO: Got Host Name.\n");

        memset(&serverAddr, 0, sizeof(serverAddr));
        //bzero((char *) &serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        bcopy((char *) remoteServer->h_addr, (char *)&serverAddr.sin_addr.s_addr,
                remoteServer->h_length);
        serverAddr.sin_port = htons(port);
        printf("INFO: Connecting. \n");

        if (connect(tcpSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
            perror("ERROR: Cannot connect to host");
            exit(0);
        }
    }

    while (1) {
        evt_le_meta_event *meta;
        le_advertising_info *info;
        char addr[18];
        int8_t rssi;
        while ((len = read(dd, buf, sizeof(buf))) < 0) {
            if (errno == EINTR && signal_received == SIGINT) {
                len = 0;
                goto done;
            }
            if (errno == EAGAIN || errno == EINTR)
                continue;
            printf("no message..\n");
            goto done;
        }
        ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
        len -= (1 + HCI_EVENT_HDR_SIZE);

        meta = (void *) ptr;

        if (meta->subevent != 0x02)
            goto done;

        /* Ignoring multiple reports */
        info = (le_advertising_info *) (meta->data + 1);
        if (check_report_filter(filter_type, info)) {
            // char name[30];

            // memset(name, 0, sizeof(name));
            ba2str(&info->bdaddr, addr);    //

            //eir_parse_name(info->data, info->length, name, sizeof(name) - 1);
            rssi = (int8_t)info->data[info->length];

            snprintf(rssistr, 15, "%d", rssi);
            current_time = microtime();// Read time
            snprintf(ctimestr,20,"%6.4f",current_time);

            mlist_t *messages = (mlist_t *) malloc(sizeof(mlist_t));
            messages->first = NULL;
            messages->last = NULL;
            messages->size = 0;

            insert(lst,rssi,current_time,addr, messages);

            if(messages != NULL){
                if(messages->size == 0){
                    free_msg_list(messages, 0);
                } 
                else if(txEnable && messages->size>0){
                    print_msg_list(messages);
                    send_message(messages,send_msg,tcpSock,readBuffer, MAX_MSG_SIZE,displayEnable,loc);
                }
            }

            /*if ( uuidEnable == 1 ){
                for(idx=0;idx<=info->length;idx++){
                    printf("%04x",(int8_t)info->data[idx]);
                }
                printf("\n");
            }

            if ( logEnable ) { //write to log
               fprintf(g_logfd, "%s\t%d\t%s\n", addr, rssi, ctimestr);
            }*/
        }
        usleep(millisecond);
    }
    done:
    setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));

    if (len < 0) {
        if ( logEnable ) {
            fclose(g_logfd);
        }
        return -1;
    }
    if ( logEnable ) {
        fclose(g_logfd);
    }
    free_beacon_list(lst);
    return 0;
}


static int update_white_list(int dd) {
    bdaddr_t ba;
    int j;
    char buff[18];
    uint8_t addr_type_r = 0x00;
    uint8_t addr_type_p = 0x01;
    char test_bt[MAX_ADDR_LEN] = { 0 };

    FILE *fp;

    fp = fopen(fileName,"r");
    // Clear white list
    printf("Adding white list\n");
    if (hci_le_clear_white_list(dd,1000)<0) {
        fclose(fp);
        return -1;
    }

    while( fgets(buff, 19, (FILE*)fp) != NULL){
        // Add to white list
        if (sizeof(buff)%18 == 0){
            if (buff[17]=='\n')
                buff[17]='\0';
            printf("%s\n",buff);
            str2ba(buff, &ba);
            ba2str(&ba, test_bt);
            printf("%s\n",test_bt);

            if (hci_le_add_white_list(dd, &ba, addr_type_r, 1000)<0){
                fclose(fp);
                return -1;
            }
            if (hci_le_add_white_list(dd, &ba, addr_type_p, 1000)<0){
                fclose(fp);
                return -1;
            }
        }
    }

    fclose(fp);
    return 0;
}

static void cmd_lescan(int dev_id, char *locstr)
{
    int err, dd;
    uint8_t own_type = 0x00;
    uint8_t scan_type = 0x00; // passive scan - not sending scan responses
    uint8_t filter_type = 0;
    uint8_t filter_policy = 0x00;
    uint16_t interval = htobs(0x0010);
    uint16_t window = htobs(0x0010);
    uint8_t filter_dup = 0;    // not filtering duplicates

    if (dev_id < 0)
        dev_id = hci_get_route(NULL);
    // Open devices
    dd = hci_open_dev(dev_id);
    if (dd < 0) {
        perror("Could not open device");
        exit(1);
    }
    // Add whitelist
    if (whiteList){
        filter_policy = 0x01;
        // Update white list before scan
        err = update_white_list(dd);
        if (err < 0) {
            perror("Set scan white list failed");
            exit(1);
        }
    }
    // Set scan parameters
    err = hci_le_set_scan_parameters(dd, scan_type, interval, window, own_type, filter_policy, 1000);
    if (err < 0) {
        perror("Set scan parameters failed");
        exit(1);
    }

    // Set enable scan
    err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 1000);
    if (err < 0) {
        perror("Enable scan failed");
        exit(1);
    }

    printf("LE Scan ...\n");

    err = print_advertising_devices(dd, filter_type, locstr);
    if (err < 0) {
        perror("Could not receive advertising events");
        exit(1);
    }

    err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 1000);
    if (err < 0) {
        perror("Disable scan failed");
        exit(1);
    }

    hci_close_dev(dd);
}

/******************************************************************************/
// Program usage
static void usage(void){
    printf("ioteye slave-agent program.\n"
           "Usage:\n");
    printf("\tbtagent -t [master_mac_address] [options]\n");
    printf("Options:\n"
           "\t-t [host_server_addr]   Master agent mac address\n"
           "\t-p [port_number]        Host server port. [Default 9999].\n"
           "\t-w                      Whitelist enable.\n"
           "\t-f [file_name]          Whitelist file.[Default \"beacon.txt]\"\n"
           "\t-l                      Enable log.\n"
           "\t-u                      UUID enable.\n"
           "\t-s                      Packet size. [Default 10]\n"
           "\t-d                      Message display enable. \n"
           "\t-s [millisecond]        Sleep for a certain millisecond. \n"
           "\t-h                      Help.\n");
}

/******************************************************************************/

int main(int argc, char** argv) {
    char hostname[100];
    char locstr[MAX_ADDR_LEN] = { 0 };

    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    struct hci_dev_info *di_ptr;
    int c;
    gethostname(hostname, sizeof(hostname));
    printf("Local Machine Hostname: %s\n", hostname);

    // flush stdout immediately
    setvbuf(stdout, NULL, _IONBF, 0);
    opterr = 0;

    while ((c = getopt (argc, argv, "la:wf:uhs:dt:")) != -1) {
        switch (c) {
            case 'a':
                txEnable = 1;
                strncpy(host, optarg, 32);   //setup host addresss
                break;
            case 'l':
                logEnable = 1;
                break;
            case 'w':
                whiteList = 1;
                break;
            case 'f':
                whiteList = 1;
                strncpy(fileName, optarg, 100);
                break;
            case 'u':
                uuidEnable = 1;
                break;
            case 'd':
                displayEnable = 1;
                break;
            case 's':
                packetSize = atoi(optarg);
                break;
            case 'h':
                usage();
                exit(0);
                break;
            case 't':
                millisecond = atoi(optarg);
                break;
            default:
                printf("-h for help.\n");
                exit(EXIT_FAILURE);
         }
    } //end of while

    // Check destination MAC address

    printf ("Host = %s, tx enable=%d, log enable=%d, whitelist = %d, uuid = %d\n",
            host, txEnable, logEnable, whiteList, uuidEnable);

    //get local BT information
    if ((masterSock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) { // Open HCI socket
        perror("Can't open HCI socket.");
        exit(EXIT_FAILURE);
    }
    if (!(dl = malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t)))) {
        perror("Can't allocate memory");
        exit(EXIT_FAILURE);
    }

    dl->dev_num = HCI_MAX_DEV;
    dr = dl->dev_req;

    if (ioctl(masterSock, HCIGETDEVLIST, (void *) dl) < 0) {
        perror("Can't get device list");
        exit(EXIT_FAILURE);
    }
    printf("Found BT local dev num=%d\n", dl->dev_num);

    di.dev_id = dr->dev_id;
    if (ioctl(masterSock, HCIGETDEVINFO, (void *) &di) < 0) {
        perror("Can't get device info");
        exit(EXIT_FAILURE);
    }

    if (hci_test_bit(HCI_RAW, &di.flags) && !bacmp(&di.bdaddr, BDADDR_ANY)) {
        int dd = hci_open_dev(di.dev_id);
        hci_read_bd_addr(dd, &di.bdaddr, 1000);
        hci_close_dev(dd);
    }

    di_ptr = &di;
    ba2str(&di_ptr->bdaddr, locstr);
    printf("Local BT MAC addr=%s\n", locstr);

    //start scan
    cmd_lescan(-1, locstr);

    return 0;
}

#if 0
//For future UUID reading

static void print_eir(const uint8_t *eir, uint8_t eir_len)
{
    uint16_t len = 0;

    if (eir_len == 0)
        return;

    while (len < eir_len - 1) {
        uint8_t field_len = eir[0];
        const uint8_t *data = &eir[2];
        uint8_t data_len;
        char name[239], label[100];
        uint8_t flags, mask;
        int i;

        // Check for the end of EIR
        if (field_len == 0)
            break;

        len += field_len + 1;

        // Do not continue EIR Data parsing if got incorrect length
        if (len > eir_len) {
            len -= field_len + 1;
            break;
        }

        data_len = field_len - 1;

        switch (eir[1]) {
            case EIR_FLAGS:
                flags = *data;
                mask = flags;

                //"Flags: 0x%2.2x", flags

                for (i = 0; eir_flags_table[i].str; i++) {
                    if (flags & (1 << eir_flags_table[i].bit)) {
                        info("  %s",
                                eir_flags_table[i].str);
                        mask &= ~(1 << eir_flags_table[i].bit);
                    }
                }
                break;

            case EIR_UUID16_SOME:
                if (data_len < sizeof(uint16_t))
                    break;
                print_uuid16_list("16-bit Service UUIDs (partial)",
                                data, data_len);
                break;

            case EIR_UUID16_ALL:
                if (data_len < sizeof(uint16_t))
                    break;
                print_uuid16_list("16-bit Service UUIDs (complete)",
                                data, data_len);
                break;

            case EIR_UUID32_SOME:
                if (data_len < sizeof(uint32_t))
                    break;
                print_uuid32_list("32-bit Service UUIDs (partial)",
                                data, data_len);
                break;

            case EIR_UUID32_ALL:
                if (data_len < sizeof(uint32_t))
                    break;
                print_uuid32_list("32-bit Service UUIDs (complete)",
                                data, data_len);
                break;

            case EIR_UUID128_SOME:
                if (data_len < 16)
                    break;
                print_uuid128_list("128-bit Service UUIDs (partial)",
                                    data, data_len);
                break;

            case EIR_UUID128_ALL:
                if (data_len < 16)
                    break;
                print_uuid128_list("128-bit Service UUIDs (complete)",
                                    data, data_len);
                break;

            case EIR_SERVICE_UUID128:
                if (data_len < 16)
                    break;
                print_uuid128_list("128-bit Service UUIDs",
                                data, data_len);
                break;

            default:
                sprintf(label, "Unknown EIR field 0x%2.2x", eir[1]);
                print_hex_field(label, data, data_len);
                break;
        }

        eir += field_len + 1;
    }

    if (len < eir_len && eir[0] != 0)
        packet_hexdump(eir, eir_len - len);
}

#endif
