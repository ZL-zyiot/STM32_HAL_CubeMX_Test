

#ifndef __DATA_UPDATE_TASK_H__
#define __DATA_UPDATE_TASK_H__
#include "main.h"
#include "update.h"


#define FRAME_BUF_SIZE		(FRAME_MAX_SIZE)

void initUpdateTask(uint8_t **psenddata, uint8_t **precvdata);
UPDATE_ERROR_TYPE_E data_Updata_Process(void);



#endif

