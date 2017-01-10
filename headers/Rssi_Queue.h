/*
 * Data structure for computation
 *
 * @Author Adam Ho
 * 
 *--------------------------
 * Visualization:
 *
 *    List_entry------>beacon01------------>beacon02------------>beacon03------>NULL
 *                         |                    |                    |
 *                        / \                  / \                  / \
 *                       /   \                /   \                /   \
 *                agent01    agent02    agent01  agent02     agent01   agent02
 *                 /            \          /         \          /         \
 *		  rssi_val01     rssi_val02  rssi_val03  rssi_val04 rssi_val05   rssi_val06
 *            |              |            |           |        |           |
 *        rssi_val07     rssi_val08  rssi_val09  rssi_val10 rssi_val11   rssi_val12
 *            |              |            |           |        |           |
 */


#include <stdlib.h>
#include <stdio.h>

typedef struct rssi
{
	int8_t rssi;
	struct rssi *next;
}rssi_t;

typedef struct rssi_queue
{
	rssi_t *head;
	rssi_t *end;
	char mac[18];
	struct rssi_queue *next;
}rssi_queue_t;

typedef struct beacon{

	rssi_queue_t *head;
	rssi_queue_t *tail;
	char mac[18];
	struct beacon *next;

}beacon_t;


