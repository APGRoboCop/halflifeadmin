/*
 * Plugin system functions
 *
 * $Id: plugin.inc,v 1.1.1.1 2001/03/13 16:00:17 darope Exp $
 *
 */
#if defined _plugin_included
  #endinput
#endif
#define _plugin_included


native plugin_register(Cmd[],Function[]);
native plugin_message(Msg[]);
native plugin_setdesc(Desc[]);
native plugin_setname(Name[]);
native plugin_setversion(Version[]);
