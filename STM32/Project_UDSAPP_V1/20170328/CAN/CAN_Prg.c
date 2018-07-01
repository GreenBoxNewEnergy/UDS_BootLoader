#include "SDF_Types.h"
#include "CAN_Types.h"
#include "CAN_Pub.h"

/* Frame status */
#define CAN_FRAME_STATUS_TX_REQ             0x01U
#define CAN_FRAME_STATUS_RX_LOST_FLAG       0x01U
#define CAN_FRAME_STATUS_TX_PROGRESS        0x02U
#define CAN_FRAME_STATUS_RX_FLAG            0x02U
#define CAN_FRAME_STATUS_PERIOD_EN          0x04U
#define CAN_FRAME_STATUS_TX_CONF            0x08U
#define CAN_MON_NODE_RELOAD                 0x08U
#define CAN_MON_NODE_TASK                   0x10U
#define CAN_MON_NODE_STATUS_OK              0x20U
#define CAN_MON_NODE_STATUS_NOK             0x00U
#define CAN_FRAME_STATUS_TX_DATA            0x80U


/*******************************************************************************
*
*       Private Variables
*
*/
/* CAN software layer status */
/* Range : (CAN_MODE_OFF, CAN_MODE_BUS_ON, CAN_MODE_BUS_OFF) */
tCanMode m_eCanMode;

const u8 m_aubCanFrameConfig[CAN_FRAME_MAX] =
{
  /* RX */
  CAN_MODE_DIRECT, 
  CAN_MODE_DIRECT, 
  /* TX */
  CAN_MODE_DIRECT,
};


u8 m_aubStatus[CAN_FRAME_MAX_RAM];                /* Status */

/**
 * Frame in transmission transmission request
 *
 * @param uFrameIdx - CAN frame index.
 * @return eStatus - function status:
 *		     - CAN_ERR_OK - no error
 *    	   - CAN_ERR_OFF - transmission is not allowed
 *    	   - CAN_ERR_IDX - frame index is out of range
 *    	   - CAN_ERR_TX_MSG_LOST - transmission already pending
 *    	   - CAN_ERR_FRAME_MODE - frame is in periodic mode
 */
tCanStatus CAN_SendFrame (tCanFrameIdx uFrameIdx)
{
  tCanStatus eStatus;
  
  eStatus = CAN_ERR_OK;
  
  switch (m_eCanMode)
  {
  case CAN_MODE_BUS_ON:
    {
      /* Transmission is allowed */
      /* The frame index input parameter is out of range or */
      /* the frame is configured in reception, then error */
      if ((uFrameIdx < CAN_RX_FRAME_MAX) || (uFrameIdx >= CAN_FRAME_MAX))
      {
        eStatus = CAN_ERR_IDX;
      }
      /* The frame is configured in transmission */
      else
      {
#ifndef BOOTLOADER
        /* The frame is configured in direct or mixed transmission, */
        /* the transmission request is memorised. */
        if ((m_aubCanFrameConfig[uFrameIdx] & CAN_MODE) != CAN_MODE_PERIODIC)
        {
          /* A transmission is pending for this frame */
          if (m_aubStatus[uFrameIdx] & CAN_FRAME_STATUS_TX_DATA)
          {
            eStatus = CAN_ERR_TX_MSG_LOST;
          }
          /* No transmission pending */
          else
          {
            /* Application interrupts disabling, to prevent variable reading or writing */
            /* when reading and writing the variable */
            //u8 ebStatus;
            //ebStatus = APP_InterruptDisable ();
            
            /* Flag set to request a transmission */
            //NCS_SET_BIT(m_aubStatus[uFrameIdx], COM_FRAME_STATUS_TX_REQ, u8);
            
            /* Application interrupts enabling, the variable is coherent */
            //APP_InterruptEnable (ebStatus);
          } /* end else (m_aubStatus[uFrameIdx] & COM_FRAME_STATUS_TX_DATA) */
        }
        
        /* The frame mode does not permit to request a transmission */
        else
        {
          eStatus = CAN_ERR_FRAME_MODE;
        }
#else
        /* The transmission mailbox is busy */
        m_ebTxMailboxStatus[uFrameIdx] = CAN_MAILBOX_BUSY;
        
        /* In Bootloader mode, strat the trasmission now (only direct transmission mode is used) */
        COM_TxFrame (uFrameIdx - CAN_TX_MAILBOX_BASE_IDX, uFrameIdx);
#endif
      } /* end else (uFrameIdx >= COM_FRAME_MAX) */
    }
    break;
  case CAN_MODE_BUS_OFF :
  case CAN_MODE_OFF :
    /* Transmission is not allowed, function ending */
    eStatus = CAN_ERR_OFF;
    break;
  default :
    m_eCanMode = m_eCanModeDefaultValue;
    //NCS_SdfVariableRangeError (SDF_COM_MODE);
    /* Transmission is not allowed, function ending */
    eStatus = CAN_ERR_COHE;
    break;
  }
  
  /* Return the status */
  return (eStatus);
}


/**
 * Request the transmission of a CAN mailbox
 *
 * @param uMailboxIdx - mailbox index.
 */
void CAN_TransmitFrame (tCanMailboxIdx uMailboxIdx)
{
  //  CAN_MO_CTR_ADDR_H(uMailboxIdx) = CAN_MO_CTR_TXRQ_BIT | CAN_MO_CTR_TXEN0_BIT | CAN_MO_CTR_TXEN1_BIT | CAN_MO_CTR_MSGVAL_BIT;
	  CAN_TX();
    CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE); 	
}





/* Declare the default value of the m_eComMode variable */
const tCanMode m_eCanModeDefaultValue = CAN_MODE_OFF;
