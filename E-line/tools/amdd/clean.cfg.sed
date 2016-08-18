\;^///////////////////////////////////////////////////////////////.\?$;,\;^///////////////////////////////////////////////////////////////.\?$;d
\;^////////////////////////////////////////////////////////////////////////////.\?$;,\;^////////////////////////////////////////////////////////////////////////////.\?$;d
/Used by the TFC plugin only. If enabled, an attempt to switch/d
/teams will be rejected if it would unbalance the teams/d
/(0=disabled, 1=enabled)/d
s/^ *\(admin_balance_teams .*\)/\/\/ \1/
/If you are using bots with admin mod/d
/It will protect bots from receiving client/d
/would crash your server/d
s/^ *\(admin_bot_protection .*\)/\/\/ \1/
/This is the message displayed to everyone after connecting/d
s/^ *\(admin_connect_msg .*\)/\/\/ \1/
/This controls the availability of weapon restriction/d
s/^ *\(admin_cs_restrict .*\)/\/\/ \1/
/This will produce debugging messages in your logs which can/d
/be used to troubleshoot problems. Not recommended for general use/d
s/^ *\(admin_debug .*\)/\/\/ \1/
/Determines whether or not the fun commands are allowed/d
/If not on by default, the admin can still turn it on when/d
s/^ *\(admin_fun_mode .*\)/\/\/ \1/
/Enable to get special effects with certain commands/d
/like teleport or slap./d
s/^ *\(admin_fx .*\)/\/\/ \1/
/If enabled, people who are gagged/d
/unable to change their name while gagged/d
s/^ *\(admin_gag_name .*\)/\/\/ \1/
/If enabled, people who are gagged will not be able to use/d
/the say_team command./d
s/^ *\(admin_gag_sayteam .*\)/\/\/ \1/
/Makes the admin with the highest access level the only admin/d
/Example: if multiple admins are present, only the one with/d
/access level will have admin access/d
s/^ *\(admin_highlander .*\)/\/\/ \1/
/If admin_ignore_immunity is enabled, ACCESS_IMMUNITY/d
/ignored and does nothing/d
s/^ *\(admin_ignore_immunity .*\)/\/\/ \1/
/This file specifies which script plugins get load/d
/It should be relative from the <mod> directory/d
s/^ *\(admin_plugin_file .*\)/\/\/ \1/
/The time (in seconds) during which an admin can reconnect/d
/disconnecting without resetting his password in the setinfo/d
s/^ *\(admin_reconnect_timeout .*\)/\/\/ \1/
/Message displayed to users who try to execute commands/d
/they don't have the appropriate access rights for/d
s/^ *\(admin_reject_msg .*\)/\/\/ \1/
/How often, in seconds, the repeat_message should be shown/d
/the screen suring the game. Minimum value is 15 seconds/d
/a value of 600 = 10 minutes between messages/d
s/^ *\(admin_repeat_freq .*\)/\/\/ \1/
/Message that is show to everyone on the server every/d
/ten minutes by the message plugin/d
s/^ *\(admin_repeat_msg .*\)/\/\/ \1/
/The old verbosity.  Defines how commands are announced/d
/<user> used command <command>/d
/0=all commands announced in chat with the admin name/d
/1=all commands announced in chat, but without the admin name/d
/2=most commands not announced at all/d
s/^ *\(admin_quiet .*\)/\/\/ \1/
/This file is used to store configuration data across maps/d
/and even across server restarts./d
s/^ *\(admin_vault_file .*\)/\/\/ \1/
/If enabled, a hlds_ld-style map vote will automatically/d
/start five minutes before the end of a map/d
s/^ *\(admin_vote_autostart .*\)/\/\/ \1/
/If set to on status (1), when a vote is in progress all/d
/players will see the votes of other players as they vote/d
s/^ *\(admin_vote_echo .*\)/\/\/ \1/
/Number of seconds that must elapse after start of the map/d
/or the end of another vote, before another hlds_ld-style/d
/map vote can be called/d
s/^ *\(admin_vote_freq .*\)/\/\/ \1/
/Controls how many times the current map can be/d
/extended for thirty minutes/d
s/^ *\(admin_vote_maxextend .*\)/\/\/ \1/
/Percent of players who have to vote for a map to get/d
/to win a hlds_ld-style map vote/d
s/^ *\(admin_vote_ratio .*\)/\/\/ \1/
/Ability to make clients execute commands/d
s/^ *\(allow_client_exec .*\)/\/\/ \1/
/If set to 1, players who try to crash the server by using/d
/non-printable characters in mid-game will be banned from/d
/the server for 24 hours. Regardless of this variable/d
/setting, offenders will be kicked if they do this/d
s/^ *\(amv_autoban .*\)/\/\/ \1/
/Default access rights for players not in the users.ini file/d
/See documentation for access levels and information/d
s/^ *\(default_access .*\)/\/\/ \1/
/encrypt_password, for LINUX ONLY, whether to use/d
/encrypted passwords or not/d
s/^ *\(encrypt_password .*\)/\/\/ \1/
/If enabled, the scripting file functions have read/d
/access to files/d
s/^ *\(file_access_read .*\)/\/\/ \1/
/If enabled, the scripting file functions have write/d
/access to files/d
s/^ *\(file_access_write .*\)/\/\/ \1/
/This is only used with the old script system and is not/d
/needed with the new plugin system/d
s/^ *\(help_file .*\)/\/\/ \1/
/Declares priority IPs that are allowed to take a /d
/reserved spot (if any are set up) without a password/d
s/^ *\(ips_file .*\)/\/\/ \1/
/Ratio of players who must vote 'yes' to a kick for/d
/it to be successful/d
s/^ *\(kick_ratio .*\)/\/\/ \1/
/Ratio of players who must vote 'yes' to a map change/d
/for it to be successful/d
s/^ *\(map_ratio .*\)/\/\/ \1/
/List of maps people are allowed to vote for. 0 to disable/d
/Disable to enable all maps and use list from mapcycle/d
s/^ *\(maps_file .*\)/\/\/ \1/
/The file (relative to the <mod> dir) that/d
/reserved models are loaded from. 0 to disable/d
s/^ *\(models_file .*\)/\/\/ \1/
/The message shown to someone who gets kicked for/d
/trying to use a reserved model/d
s/^ *\(models_kick_msg .*\)/\/\/ \1/
/The message shown to someone who gets kicked for/d
/trying to use a reserved nickname/d
s/^ *\(nicks_kick_msg .*\)/\/\/ \1/
/first password of the setinfo line/d
/If password_field is "pw-AdminMod"/d
/setinfo "pw-AdminMod" "password-in-users.ini"/d
s/^ *\(password_field .*\)/\/\/ \1/
/If pretty_say is enabled, centersay() fades in/d
/and out and does some other tricks/d
s/^ *\(pretty_say .*\)/\/\/ \1/
/Controls how many (number) of the server's slots are reserved/d
/This is useful only if reserve_type, below, is either/d
s/^ *\(reserve_slots .*\)/\/\/ \1/
/Custom message given to clients trying to connect/d
/a reserved slots, when no public slots are free/d
s/^ *\(reserve_slots_msg .*\)/\/\/ \1/
/This controls how reserve slots work on the server/d
s/^ *\(reserve_type .*\)/\/\/ \1/
/If you have "admin_plugin_file" set above, this CVAR is ignored/d
/Define the path to the script file here, if you don't want/d
/to use the plugin system./d
s/^ *\(script_file .*\)/\/\/ \1/
/If enabled, names are compared to those who have privileges/d
/with regular expressions/d
s/^ *\(use_regex .*\)/\/\/ \1/
/The file in which you define your admins, their/d
/and the access levels that they are assigned to/d
s/^ *\(users_file .*\)/\/\/ \1/
/The minimum number of seconds allowed between votes/d
/called with the vote() (admin_vote functions) scripting/d
/If 0 or disabled, the vote() scripting function is disabled/d
s/^ *\(vote_freq .*\)/\/\/ \1/
/Location of word filter file. 0 to disable/d
/like "wordlist.txt" if enabled/d
s/^ *\(words_file .*\)/\/\/ \1/
/(0=disable, 1=enabled)/d
s/^ *\(mysql_host .*\)/\/\/ \1/
s/^ *\(mysql_user .*\)/\/\/ \1/
s/^ *\(mysql_pass .*\)/\/\/ \1/
s/^ *\(mysql_dbtable_users .*\)/\/\/ \1/
s/^ *\(mysql_dbtable_words .*\)/\/\/ \1/
s/^ *\(mysql_dbtable_models .*\)/\/\/ \1/
s/^ *\(mysql_dbtable_tags .*\)/\/\/ \1/
s/^ *\(mysql_dbtable_users .*\)/\/\/ \1/
s/^ *\(mysql_dbtable_tags .*\)/\/\/ \1/
s/^ *\(mysql_users_sql .*\)/\/\/ \1/
s/^ *\(mysql_tags_sql .*\)/\/\/ \1/
s/^ *\(mysql_preload .*\)/\/\/ \1/
s/^ *\(amv_private_server .*\)/\/\/ \1/
s/^ *\(amv_anti_cheat_options .*\)/\/\/ \1/
s/^ *\(amv_reconnect_time .*\)/\/\/ \1/
s/^ *\(amv_enable_beta .*\)/\/\/ \1/
s/^ *\(amv_log_passwords .*\)/\/\/ \1/
s/^ *\(admin_command .*\)/\/\/ \1/