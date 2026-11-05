#pragma once

#if defined(APP_NODE_TX)
#define APP_NODE_IS_TX 1U
#define APP_NODE_IS_RX 0U
#elif defined(APP_NODE_RX)
#define APP_NODE_IS_TX 0U
#define APP_NODE_IS_RX 1U
#else
#error "Node role not defined. Build with make tx or make rx"
#endif
