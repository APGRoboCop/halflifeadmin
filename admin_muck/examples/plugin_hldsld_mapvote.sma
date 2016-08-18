/* This plugin contains commands specific for the Team Fortress Classic mod */

/* $Id $ */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_HLDSLD_VOTE 1
#define ACCESS_CONTROL_VOTE 2

#define MAP_INVALID -1
#define MAX_MAPS 64

enum VoteStatus {
	VoteInProgress,		/* Voting is allowed */
	VoteNotBegun,		/* Voting not allowed - no vote in progress, but a vote may be called */
	MapStart,			/* Voting not allowed - map just began */
	VoteFinished		/* Voting not allowed - vote just finished */
}

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

/* Key word to use to extend map during map vote */
new MAP_EXTEND[] = "extend";

new ExtendCount = 0;
new ExtendMapTime = 0;
new VoteStatus:MapVoteStatus = MapStart;

/* 2 dimensional array - Map Index / Map Name */
new Maps[MAX_MAPS][MAX_NAME_LENGTH]; 
new UserVote[MAX_PLAYERS] = {MAP_INVALID,...};

/* Given a map name, returns the index associated with the map.  Returns MAP_INVALID if no
new index can be created.  This should never happen, though. */
AddMapIndex(MapName[]) {
	new Text[MAX_TEXT_LENGTH];
	new MapCount = 0;
	new MapIndex = GetMapIndex(MapName);
	
	if (MapIndex==MAP_INVALID) {
		for(MapCount=0;MapCount<MAX_MAPS;MapCount++) {
			if (Maps[MapCount][0] == NULL_CHAR) {
				MapIndex = MapCount;
				strcpy(Maps[MapIndex],MapName, MAX_NAME_LENGTH);
				break;
			}
		}
	}
	if (MapIndex==MAP_INVALID) {
		snprintf(Text, MAX_TEXT_LENGTH, "WARNING: Found no map index for %s.", MapName);
		say(Text);
	}
	return MapIndex;
}

/* Give the map with the highest number of votes so far */
CalculateWinningMap(Speak) {
	new iMap;
	new iPlayer;
	new iRatio = getvar("admin_vote_ratio");
	new iWinner = MAP_INVALID;
	new MapText[MAX_NAME_LENGTH];
	new MapVotes[MAX_MAPS] = {0,...};
	new MaxPlayers = maxplayercount();
	new MaxVotes = 0;
	new Text[MAX_TEXT_LENGTH];
	new VoteName[MAX_NAME_LENGTH];
	
	if (iRatio > 0) {
		MaxVotes = (playercount() * iRatio) / 100;
		
		if (MaxVotes > 0)
			MaxVotes--;
	}
	
	for(iPlayer=0; iPlayer<=MaxPlayers; iPlayer++) {
		iMap = UserVote[iPlayer];
		if (iMap != MAP_INVALID) {
			MapVotes[iMap] = MapVotes[iMap] + 1;
			if(MapVotes[iMap] > MaxVotes) {
				MaxVotes = MapVotes[iMap];
				iWinner = iMap;
			}
		}
	}
	
	if(Speak==1) {
		if(iWinner!=MAP_INVALID) {

			if(streq(Maps[iWinner],MAP_EXTEND)==1) {
    	  new iExtend = GetExtendTime();
    	  snprintf(MapText, MAX_NAME_LENGTH, "Extend for %i minutes", iExtend);
			} else {
				strcpy(MapText, Maps[iWinner], MAX_NAME_LENGTH);
			}
			if(MaxVotes == 1) {
				strcpy(VoteName, "vote", MAX_NAME_LENGTH);
			} else {
				strcpy(VoteName, "votes", MAX_NAME_LENGTH);
			}
			snprintf(Text, MAX_TEXT_LENGTH, "%s is winning with %i %s.", MapText, MaxVotes, VoteName);
			say(Text);
		} else {
			snprintf(Text, MAX_TEXT_LENGTH, "No map has the %i votes required yet.", ++MaxVotes);
			say(Text);
		}
	}
	return iWinner;
}

CancelVoteHelper(Public = 0) {
  new iFreq = getvar("admin_vote_freq");
  
  if (check_auth(ACCESS_CONTROL_VOTE)==0) {
		reject_message(Public);
		return PLUGIN_HANDLED;
	}

  if(MapVoteStatus == VoteInProgress) {
  	say("Voting has been cancelled!");
  	MapVoteStatus = VoteFinished;
  	if (iFreq > 0) {
  	  set_timer("AllowMapVote",iFreq,1);
  	}
  } else {
  	if (Public == 0) {
  		selfmessage("There is no map vote in progress.");
  	} else {
  		say("There is no map vote in progress.");
  	}
  }
  return PLUGIN_HANDLED;
}

DenyMapHelper(Public = 0, Map[]) {
	new i;
	new iMapIndex;
	new iVotesRemoved = 0;
	new Text[MAX_TEXT_LENGTH];
	
	if (check_auth(ACCESS_CONTROL_VOTE)==0) {
		reject_message(Public);
		return PLUGIN_HANDLED;
	}

	if(MapVoteStatus==VoteInProgress) {
		iMapIndex = GetMapIndex(Map);
		if (iMapIndex == MAP_INVALID) {
			if (Public == 0) {
				selfmessage("There are no votes for that map.");
			} else {
				say("There are no votes for that map.");
			}
		} else {
			for (i = 0; i < MAX_PLAYERS; i++) {
				if (UserVote[i] == iMapIndex) {
					UserVote[i] = MAP_INVALID;
					iVotesRemoved++;
				}
			}
			strinit(Maps[iMapIndex]);
			if (iVotesRemoved == 1) {
			  snprintf(Text, MAX_TEXT_LENGTH, "1 vote for %s removed.", Map);
			} else {
			  snprintf(Text, MAX_TEXT_LENGTH, "%i votes for %s removed.", iVotesRemoved, Map);
			}
			say(Text);
		}
	} else {
		if (Public == 0) {
			selfmessage("There is no map vote in progress.");
		} else {
			say("There is no map vote in progress.");
		}
	}
	return PLUGIN_HANDLED;
}

/* Return the amount of time (in minutes) to extend a map for if an extend vote wins. */
GetExtendTime() {
	/* If this is the first time we're extending the map, we want to extend it
	by the length of mp_timelimit.  Second and subsequent times through, we want
	to keep extending it by the original timelimit...not the new one (eg, if we
	start at 30 mins...on the second time through, mp_timelimit will say 60.  We
	want to jump to 90, not 120. */
	if (ExtendMapTime == 0) {
		ExtendMapTime = getvar("mp_timelimit");
	}
	return ExtendMapTime;
}

/* Like AddMapIndex, but won't create a new entry if one doesn't exist...will simply return MAP_INVALID. */
GetMapIndex(MapName[]) {
	new MapCount = 0;
	new MapIndex = MAP_INVALID;
	
	/*  Find our Map within the list of previous votes.  If the Map doesn't exist, add 'em. */
	for(MapCount=0;MapCount<MAX_MAPS;MapCount++) {
		/* Check to see if the map at the current index is the one we want. */
		if (streq(MapName, Maps[MapCount])==1) {
			MapIndex = MapCount;
			break;
		}
	}
	return MapIndex;
}

/* Given a user and a map, processes the vote.  This includes subtracting a previous vote, if
the user had already made one. */
ProcessVote(User[],UserIndex,MapName[]) {
	new iMap;
	new iNewMap = 0;
	new iPlayer;
	new iVoteCount = 0;
	new MapText[MAX_NAME_LENGTH];
	new MaxPlayers = maxplayercount();
	new Text[MAX_TEXT_LENGTH];
	new VoteName[MAX_NAME_LENGTH];

	iNewMap = AddMapIndex(MapName);
	if (iNewMap == MAP_INVALID) {
		snprintf(Text, MAX_TEXT_LENGTH, "Oops. Could not get an index for %s.", MapName);
		say(Text);
		return;
	}

	iMap = UserVote[UserIndex];
	if (iMap != MAP_INVALID) {
		snprintf(Text, MAX_TEXT_LENGTH, "Removing previous vote for %s.", Maps[iMap]);
		say(Text);
	}
	UserVote[UserIndex] = iNewMap;

	for(iPlayer=0; iPlayer<=MaxPlayers; iPlayer++) {
		iMap = UserVote[iPlayer];
		if (iMap==iNewMap)
			iVoteCount++;
	}
	
	if(streq(Maps[iNewMap],MAP_EXTEND)==1) {
	  new iExtend = GetExtendTime();
	  snprintf(MapText, MAX_NAME_LENGTH, "Extend for %i minutes", iExtend);
	} else {
		strcpy(MapText, Maps[iNewMap], MAX_NAME_LENGTH);
	}
	if(iVoteCount == 1) {
		strcpy(VoteName, "vote", MAX_NAME_LENGTH);
	} else {
		strcpy(VoteName, "votes", MAX_NAME_LENGTH);
	}
	snprintf(Text, MAX_TEXT_LENGTH, "Vote by %s -- %s now has %i %s.", User, MapText, iVoteCount, VoteName);
	say(Text);
}

/* Resets the vote data to newly initialized states. */
ResetVoteData() {
	new i;
	for(i=0;i<MAX_MAPS;i++) {
		strinit(Maps[i]);
	}
	for(i=0;i<MAX_PLAYERS;i++) {
		UserVote[i] = MAP_INVALID;
	}
}

StartVoteHelper(Public = 0) {
	new Text[MAX_TEXT_LENGTH];
	
	if (check_auth(ACCESS_HLDSLD_VOTE) == 0) {
		reject_message(Public);
		return PLUGIN_HANDLED;
	}
	
	if (getvar("admin_vote_freq") <= 0) {
	  if (check_auth(ACCESS_CONTROL_VOTE) == 0) {
	    reject_message(Public);
	    return PLUGIN_HANDLED;
	  }
	}

	if (MapVoteStatus==VoteInProgress) {
		strcpy(Text,"A map vote is already in progress.",MAX_TEXT_LENGTH);
	} else if (MapVoteStatus==VoteNotBegun || check_auth(ACCESS_CONTROL_VOTE)==1) {
		StartMapVote();
		return PLUGIN_HANDLED;
	} else if (MapVoteStatus==MapStart) {
		new iFreq = getvar("admin_vote_freq") / 60;
		snprintf(Text,MAX_TEXT_LENGTH,"A map vote is not allowed within %i minutes of the map start.",iFreq);
	} else if (MapVoteStatus==VoteFinished) {
		new iFreq = getvar("admin_vote_freq") / 60;
		snprintf(Text,MAX_TEXT_LENGTH,"A map vote is not allowed within %i minutes of a previous vote.",iFreq);
	} else {
		strcpy(Text,"Oops.  Apparently, there's an unhandled map vote status here.",MAX_TEXT_LENGTH);
	}
	if (Public == 0) {
		selfmessage(Text);
	} else {
		say(Text);
	}
	return PLUGIN_HANDLED;
}

public AllowMapVote(Timer,Repeat,HLUser,HLParam) {
	MapVoteStatus = VoteNotBegun;
}

public admin_cancelvote(HLCommand,HLData,HLUserName,UserIndex) {
	CancelVoteHelper(0);
	return PLUGIN_HANDLED;
}

public admin_denymap(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	DenyMapHelper(0,Data);
	return PLUGIN_HANDLED;
}

public admin_startvote(HLCommand,HLData,HLUserName,UserIndex) {
	StartVoteHelper(0);
	return PLUGIN_HANDLED;
}

public ChangeMap(Timer,Repeat,HLUser,HLParam) {
	new NewMap[MAX_NAME_LENGTH];
	
	convert_string(HLParam,NewMap,MAX_NAME_LENGTH);
	if(strlen(NewMap) != 0) {
		changelevel(NewMap);
	}
}

public EndMapVote(Timer,Repeat,HLUser,HLParam) {
	new iWinner = MAP_INVALID;
	new Text[MAX_TEXT_LENGTH];
	
	if (MapVoteStatus!=VoteInProgress) 
		return;
		
	iWinner = CalculateWinningMap(0);
	if (iWinner==MAP_INVALID) {
		strcpy(Text, "Voting is now over. No map got enough votes to win.", MAX_TEXT_LENGTH);
		say(Text);
	} else if (streq(Maps[iWinner],MAP_EXTEND)==1) {
    new iExtend = GetExtendTime();
    snprintf(Text, MAX_TEXT_LENGTH, "Voting is now over. The map will be extended for %i more minutes.", iExtend);
    say(Text);
    set_timer("ExtendMap",2,1);
	} else {
		snprintf(Text, MAX_TEXT_LENGTH, "Changing map to %s.", Maps[iWinner]);
		say(Text);
		exec("mp_timelimit 1");
		set_timer("ChangeMap",2,1,Maps[iWinner]);
	}
	MapVoteStatus = VoteFinished;
}

public ExtendMap(Timer,Repeat,HLUser,HLParam) {
  new iFreq = getvar("admin_vote_freq");
  new ExecCommand[MAX_DATA_LENGTH];
  new Timelimit = 0;
	
  ExtendCount++;
  Timelimit = getvar("mp_timelimit");
  Timelimit += GetExtendTime();
  snprintf(ExecCommand, MAX_DATA_LENGTH, "mp_timelimit %i", Timelimit);
  exec(ExecCommand);
  if (iFreq > 0) {
    set_timer("AllowMapVote",iFreq,1); 
  }
  if(getvar("admin_vote_autostart") != 0) {
  	/* Call for a vote five minutes before map ends */
  	set_timer("StartMapVote",(ExtendMapTime - 5) * 60,1);
  }
}

public HandleSay(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	new i;
	new Length;
	new strMap[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_DATA_LENGTH);
	strstripquotes(Data);
	Length = strlen(Data);
	if (streq(Data, "rockthevote")==1 || streq(Data,"mapvote")==1) {
		StartVoteHelper(1);
	} else if (streq(Data, "cancelvote")==1) {
		CancelVoteHelper(1);
	} else if (strmatch(Data, "denymap ", strlen("denymap "))==1) {
		/* we need to strip out 'denymap ' (8 characters */
		for(i=8;i<Length+1;i++)
			strMap[i-8] = Data[i];
		strMap[i-8] = NULL_CHAR;
		DenyMapHelper(1, strMap);
	} else if(strmatch(Data,"vote ",strlen("vote "))==1) {

		if(MapVoteStatus!=VoteInProgress) {
			say("There is no map vote in progress.");
			return PLUGIN_CONTINUE;
		}
				
		/* we need to strip out 'vote ' (5 characters */
		for(i=5;i<Length+1;i++)
			strMap[i-5] = Data[i];
		strMap[i-5] = NULL_CHAR;
				
		if(streq(strMap,MAP_EXTEND)==1) {
			new iMaxExtend = getvar("admin_vote_maxextend");
			if(ExtendCount >= iMaxExtend) {
				snprintf(Text, MAX_TEXT_LENGTH, "Sorry, %s, the map can not be extended any longer.", User);
				say(Text);
			} else {
				ProcessVote(User,UserIndex,strMap);
				CalculateWinningMap(1);
			}
		} else if(valid_map(strMap)==1) {
			ProcessVote(User,UserIndex, strMap);
			CalculateWinningMap(1);
		} else {
			snprintf(Text, MAX_TEXT_LENGTH, "Sorry, %s, that map was not found on this server.", User);
			say(Text);
		}
	}
	return PLUGIN_CONTINUE;
}

public StartMapVote() {
	new Text[MAX_TEXT_LENGTH];
	
	/* If we've already got a map vote in progress (ie, one was called),
	don't start another one. */
	if(MapVoteStatus==VoteInProgress) {
		return PLUGIN_HANDLED;
	}
	ResetVoteData();
	say("Map voting has been enabled for 3 minutes.  Say 'vote <mapname>' to vote.");
	new iMaxExtend = getvar("admin_vote_maxextend");
	if(ExtendCount < iMaxExtend || iMaxExtend == 0) {
    new iExtend = GetExtendTime();
    snprintf(Text, MAX_TEXT_LENGTH, "Say 'vote %s' to keep playing this map for %i more minutes.", MAP_EXTEND, iExtend);
    say(Text);
	}
	MapVoteStatus = VoteInProgress;
	set_timer("EndMapVote",180,1);
	return PLUGIN_HANDLED;
}

public plugin_connect(HLUserName, HLIP, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		UserVote[UserIndex] = MAP_INVALID;
	}
	return PLUGIN_CONTINUE;
}

public plugin_disconnect(HLUserName, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		UserVote[UserIndex] = MAP_INVALID;
	}
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Admin hlds_ld-style Map Vote Plugin","Runs a chat-based interface map vote, similar to hlds_ld.",STRING_VERSION);
	
	plugin_registercmd("admin_cancelvote","admin_cancelvote",ACCESS_CONTROL_VOTE,"admin_cancelvote: Cancels the current hlds_ld vote.");
	plugin_registercmd("admin_denymap","admin_denymap",ACCESS_CONTROL_VOTE,"admin_denymap <map>: Removes all votes for map.");
	plugin_registercmd("admin_startvote","admin_startvote",ACCESS_HLDSLD_VOTE,"admin_startvote: Starts an hlds_ld vote.");
	plugin_registercmd("say","HandleSay",ACCESS_ALL);
	plugin_registerhelp("say",ACCESS_CONTROL_VOTE,"say cancelvote: Cancels the current hlds_ld vote.");
	plugin_registerhelp("say",ACCESS_CONTROL_VOTE,"say denymap <map>: Removes all votes for map.");
	plugin_registerhelp("say",ACCESS_HLDSLD_VOTE,"say mapvote: Starts an hlds_ld vote.");
	plugin_registerhelp("say",ACCESS_HLDSLD_VOTE,"say rockthevote: Starts an hlds_ld vote.");
	plugin_registerhelp("say",ACCESS_HLDSLD_VOTE,"say vote <map>: Places a vote for the map.");
	
	new iFreq = getvar("admin_vote_freq");
	new intTimeLimit = getvar("mp_timelimit") * 60;
	new intTime = timeleft(0);
	
	/* If the map is not yet iFreq seconds in, disable voting until it is */
	MapVoteStatus = MapStart;
	if (iFreq > 0 && intTime > (intTimeLimit - iFreq)) {
		set_timer("AllowMapVote",intTime - (intTimeLimit - iFreq), 1);
	}
	if(getvar("admin_vote_autostart") != 0) {
		if (intTime > 300) {
			/* Call for a vote five minutes before map ends */
			set_timer("StartMapVote", intTime - 300, 1); 
		}
	}
	
	return PLUGIN_CONTINUE;
}
