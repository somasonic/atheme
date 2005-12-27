/*
 * Copyright (c) 2005 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Module listing.
 *
 * $Id: modlist.c 4219 2005-12-27 17:41:18Z jilles $
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"operserv/modlist", FALSE, _modinit, _moddeinit,
	"$Id: modlist.c 4219 2005-12-27 17:41:18Z jilles $",
	"Atheme Development Group <http://www.atheme.org>"
);

static void os_cmd_modlist(char *origin);

command_t os_modlist = { "MODLIST", "Lists loaded modules.",
			 PRIV_SERVER_AUSPEX, os_cmd_modlist };

list_t *os_cmdtree;
extern list_t modules;

void _modinit(module_t *m)
{
	os_cmdtree = module_locate_symbol("operserv/main", "os_cmdtree");
	command_add(&os_modlist, os_cmdtree);
}

void _moddeinit()
{
	command_delete(&os_modlist, os_cmdtree);
}

static void os_cmd_modlist(char *origin)
{
	node_t *n;
	uint16_t i = 0;
	notice(opersvs.nick, origin, "Loaded modules:");

	LIST_FOREACH(n, modules.head)
	{
		module_t *m = n->data;

		notice(opersvs.nick, origin, "%2d: %-20s [loaded at 0x%lx]",
			++i, m->header->name, m->address);
	}

	notice(opersvs.nick, origin, "\2%d\2 modules loaded.", i);
	logcommand(opersvs.me, user_find(origin), CMDLOG_GET, "MODLIST");
}
