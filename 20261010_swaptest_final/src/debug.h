/**
 * \file debug.h \brief debugging module
 */

#ifndef __debug_h
#define __debug_h

#define DEBUG_MODULE(_modname)
#define DEBUG_WITH_TIME

void __debug(const char *format, ...);

#define dprintf(_format, ...)  __debug(_format, ## __VA_ARGS__)
#define irqdebug(_format, ...)  __debug(_format "\n", ## __VA_ARGS__)
#define debug(_format, ...)  __debug(_format "\n", ## __VA_ARGS__)
#define error(_format, ...) __debug("***ERROR*** "_format "\n", ## __VA_ARGS__)
#define warning(_format, ...) __debug("***WARNING*** " _format "\n", ## __VA_ARGS__)
#define info(_format, ...) __debug(_format "\n", ## __VA_ARGS__)

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define debugpoint() debug(__FILE__ "::" S__LINE__);
#define errorpoint() debug("ERROR:" __FILE__ "::" S__LINE__);

#endif  // debug.h
