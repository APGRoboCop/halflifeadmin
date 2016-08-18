\;^///////////////////////////////////////////////////////////////.\?$;,\;^///////////////////////////////////////////////////////////////.\?$;d
\;^////////////////////////////////////////////////////////////////////////////.\?$;,\;^////////////////////////////////////////////////////////////////////////////.\?$;d
/Used by the TFC plugin only. If enabled, an attempt to switch/d
/teams will be rejected if it would unbalance the teams/d
/(0=disabled, 1=enabled)/d
/admin_balance_teams/d
/If you are using bots with admin mod/d
/It will protect bots from receiving client/d
/would crash your server/d
/admin_bot_protection/d
/This is the message displayed to everyone after connecting/d
/admin_connect_msg/d
/This controls the availability of weapon restriction/d
/admin_cs_restrict/d
/This will produce debugging messages in your logs which can/d
/be used to troubleshoot problems. Not recommended for general use/d
/admin_debug/d
/Determines whether or not the fun commands are allowed/d
/If not on by default, the admin can still turn it on when/d
/admin_fun_mode/d
/Enable to get special effects with certain commands/d
/like teleport or slap./d
/admin_fx/d
/If enabled, people who are gagged/d
/unable to change their name while gagged/d
/admin_gag_name/d
/If enabled, people who are gagged will not be able to use/d
/the say_team command./d
/admin_gag_sayteam/d
/Makes the admin with the highest access level the only admin/d
/Example: if multiple admins are present, only the one with/d
/access level will have admin access/d
/admin_highlander/d
/If admin_ignore_immunity is enabled, ACCESS_IMMUNITY/d
/ignored and does nothing/d
/admin_ignore_immunity/d
/This file specifies which script plugins get load/d
/It should be relative from the <mod> directory/d
/admin_plugin_file/d
/The time (in seconds) during which an admin can reconnect/d
/disconnecting without resetting his password in the setinfo/d
/admin_reconnect_timeout/d
/Message displayed to users who try to execute commands/d
/they don't have the appropriate access rights for/d
/admin_reject_msg/d
/How often, in seconds, the repeat_message should be shown/d
/the screen suring the game. Minimum value is 15 seconds/d
/a value of 600 = 10 minutes between messages/d
/admin_repeat_freq/d
/Message that is show to everyone on the server every/d
/ten minutes by the message plugin/d
/admin_repeat_msg/d
/The old verbosity.  Defines how commands are announced/d
/<user> used command <command>/d
/0=all commands announced in chat with the admin name/d
/1=all commands announced in chat, but without the admin name/d
/2=most commands not announced at all/d
/admin_quiet/d
/This file is used to store configuration data across maps/d
/and even across server restarts./d
/admin_vault_file/d
/If enabled, a hlds_ld-style map vote will automatically/d
/start five minutes before the end of a map/d
/admin_vote_autostart/d
/If set to on status (1), when a vote is in progress all/d
/players will see the votes of other players as they vote/d
/admin_vote_echo/d
/Number of seconds that must elapse after start of the map/d
/or the end of another vote, before another hlds_ld-style/d
/map vote can be called/d
/admin_vote_freq/d
/Controls how many times the current map can be/d
/extended for thirty minutes/d
/admin_vote_maxextend/d
/Percent of players who have to vote for a map to get/d
/to win a hlds_ld-style map vote/d
/admin_vote_ratio/d
/Ability to make clients execute commands/d
/allow_client_exec/d
/If set to 1, players who try to crash the server by using/d
/non-printable characters in mid-game will be banned from/d
/the server for 24 hours. Regardless of this variable/d
/setting, offenders will be kicked if they do this/d
/amv_autoban/d
/Default access rights for players not in the users.ini file/d
/See documentation for access levels and information/d
/default_access/d
/encrypt_password, for LINUX ONLY, whether to use/d
/encrypted passwords or not/d
/encrypt_password/d
/If enabled, the scripting file functions have read/d
/access to files/d
/file_access_read/d
/If enabled, the scripting file functions have write/d
/access to files/d
/file_access_write/d
/This is only used with the old script system and is not/d
/needed with the new plugin system/d
/help_file/d
/Declares priority IPs that are allowed to take a /d
/reserved spot (if any are set up) without a password/d
/ips_file/d
/Ratio of players who must vote 'yes' to a kick for/d
/it to be successful/d
/kick_ratio/d
/Ratio of players who must vote 'yes' to a map change/d
/for it to be successful/d
/map_ratio/d
/List of maps people are allowed to vote for. 0 to disable/d
/Disable to enable all maps and use list from mapcycle/d
/maps_file/d
/The file (relative to the <mod> dir) that/d
/reserved models are loaded from. 0 to disable/d
/models_file/d
/The message shown to someone who gets kicked for/d
/trying to use a reserved model/d
/models_kick_msg/d
/The message shown to someone who gets kicked for/d
/trying to use a reserved nickname/d
/nicks_kick_msg/d
/first password of the setinfo line/d
/If password_field is "pw-AdminMod"/d
/setinfo "pw-AdminMod" "password-in-users.ini"/d
/password_field/d
/If pretty_say is enabled, centersay() fades in/d
/and out and does some other tricks/d
/pretty_say/d
/Controls how many (number) of the server's slots are reserved/d
/This is useful only if reserve_type, below, is either/d
/reserve_slots/d
/Custom message given to clients trying to connect/d
/a reserved slots, when no public slots are free/d
/reserve_slots_msg/d
/This controls how reserve slots work on the server/d
/reserve_type/d
/If you have "admin_plugin_file" set above, this CVAR is ignored/d
/Define the path to the script file here, if you don't want/d
/to use the plugin system./d
/script_file/d
/If enabled, names are compared to those who have privileges/d
/with regular expressions/d
/use_regex/d
/The file in which you define your admins, their/d
/and the access levels that they are assigned to/d
/users_file/d
/The minimum number of seconds allowed between votes/d
/called with the vote() (admin_vote functions) scripting/d
/If 0 or disabled, the vote() scripting function is disabled/d
/vote_freq/d
/Location of word filter file. 0 to disable/d
/like "wordlist.txt" if enabled/d
/words_file/d
/(0=disable, 1=enabled)/d
/mysql_host/d
/mysql_user/d
/mysql_pass/d
/mysql_dbtable_users/d
/mysql_dbtable_words/d
/mysql_dbtable_models/d
/mysql_dbtable_tags/d
/mysql_dbtable_users/d
/mysql_dbtable_tags/d
/mysql_users_sql/d
/mysql_tags_sql/d
/mysql_preload/d
/amv_private_server/d
/amv_anti_cheat_options/d
/amv_reconnect_time/d
/amv_enable_beta/d
/amv_log_passwords/d
/admin_command/d