/*
 * Copyright (c) 2003-2004 E. Will et al.
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file contains functionality which implements the OService RAW command.
 *
 * $Id: raw.c 4219 2005-12-27 17:41:18Z jilles $
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"operserv/raw", FALSE, _modinit, _moddeinit,
	"$Id: raw.c 4219 2005-12-27 17:41:18Z jilles $",
	"Atheme Development Group <http://www.atheme.org>"
);

static void os_cmd_raw(char *origin);

command_t os_raw = { "RAW", "Sends data to the uplink.",
                        PRIV_ADMIN, os_cmd_raw };

list_t *os_cmdtree;
list_t *os_helptree;

void _modinit(module_t *m)
{
	os_cmdtree = module_locate_symbol("operserv/main", "os_cmdtree");
	os_helptree = module_locate_symbol("operserv/main", "os_helptree");

        command_add(&os_raw, os_cmdtree);
	help_addentry(os_helptree, "RAW", "help/oservice/raw", NULL);
}

void _moddeinit()
{
	command_delete(&os_raw, os_cmdtree);
	help_delentry(os_helptree, "RAW");
}

static void os_cmd_raw(char *origin)
{
	char *s = strtok(NULL, "");

	if (!config_options.raw)
		return;

	if (!s)
	{
		notice(opersvs.nick, origin, "Insufficient parameters for \2RAW\2.");
		notice(opersvs.nick, origin, "Syntax: RAW <parameters>");
		return;
	}

	snoop("RAW: \"%s\" by \2%s\2", s, origin);
	logcommand(opersvs.me, user_find(origin), CMDLOG_ADMIN, "RAW %s", s);
	sts("%s", s);
}
