/*
 * Copyright (c) 2005 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * This file contains code for the CService DROP function.
 *
 * $Id: drop.c 4219 2005-12-27 17:41:18Z jilles $
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"chanserv/drop", FALSE, _modinit, _moddeinit,
	"$Id: drop.c 4219 2005-12-27 17:41:18Z jilles $",
	"Atheme Development Group <http://www.atheme.org>"
);

static void cs_cmd_drop(char *origin);

command_t cs_drop = { "DROP", "Drops a channel registration.",
                        AC_NONE, cs_cmd_drop };

list_t *cs_cmdtree;
list_t *cs_helptree;

void _modinit(module_t *m)
{
	cs_cmdtree = module_locate_symbol("chanserv/main", "cs_cmdtree");
	cs_helptree = module_locate_symbol("chanserv/main", "cs_helptree");

        command_add(&cs_drop, cs_cmdtree);
	help_addentry(cs_helptree, "DROP", "help/cservice/drop", NULL);
}

void _moddeinit()
{
	command_delete(&cs_drop, cs_cmdtree);
	help_delentry(cs_helptree, "DROP");
}

static void cs_cmd_drop(char *origin)
{
	uint32_t i;
	user_t *u = user_find(origin);
	myuser_t *mu;
	mychan_t *mc, *tmc;
	node_t *n;
	char *name = strtok(NULL, " ");

	if (!name)
	{
		notice(chansvs.nick, origin, "Insufficient parameters specified for \2DROP\2.");
		notice(chansvs.nick, origin, "Syntax: DROP <#channel>");
		return;
	}

	if (*name != '#')
	{
		notice(chansvs.nick, origin, "Invalid parameters specified for \2DROP\2.");
		notice(chansvs.nick, origin, "Syntax: DROP <#channel>");
		return;
	}

	if (!(mc = mychan_find(name)))
	{
		notice(chansvs.nick, origin, "\2%s\2 is not registered.", name);
		return;
	}

	if (!is_founder(mc, u->myuser) && !has_priv(u, PRIV_CHAN_ADMIN))
	{
		notice(chansvs.nick, origin, "You are not authorized to perform this operation.");
		return;
	}

	if (metadata_find(mc, METADATA_CHANNEL, "private:close:closer") && !has_priv(u, PRIV_CHAN_ADMIN))
	{
		logcommand(chansvs.me, u, CMDLOG_REGISTER, "%s failed DROP (closed)", mc->name);
		notice(chansvs.nick, origin, "The channel \2%s\2 is closed; it cannot be dropped.", mc->name);
		return;
	}

	if (!is_founder(mc, u->myuser))
	{
		logcommand(chansvs.me, u, CMDLOG_ADMIN, "%s DROP", mc->name);
		wallops("%s dropped the channel \2%s\2", origin, name);
	}
	else
		logcommand(chansvs.me, u, CMDLOG_REGISTER, "%s DROP", mc->name);

	snoop("DROP: \2%s\2 by \2%s\2 as \2%s\2", mc->name, u->nick, u->myuser->name);

	if ((config_options.chan && irccasecmp(mc->name, config_options.chan)) || !config_options.chan)
		part(mc->name, chansvs.nick);
	hook_call_event("channel_drop", mc);
	mychan_delete(mc->name);

	notice(chansvs.nick, origin, "The channel \2%s\2 has been dropped.", name);
	return;
}
