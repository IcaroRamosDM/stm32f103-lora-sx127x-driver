#pragma once

#define SX127X_REG_FIFO         0x00U
#define SX127X_REG_OP_MODE      0x01U
#define SX127X_REG_VERSION      0x42U

#define SX127X_WRITE_MASK       0x80U
#define SX127X_READ_MASK        0x7FU

#define SX127X_REG_FRF_MSB      0x06U
#define SX127X_REG_FRF_MID      0x07U
#define SX127X_REG_FRF_LSB      0x08U

#define SX127X_MODE_SLEEP       0x00U
#define SX127X_MODE_STDBY       0x01U
#define SX127X_LONG_RANGE_MODE  0x80U
