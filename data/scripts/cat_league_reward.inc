@ Cat League Challenge: NPC reward flow and editable dialogue strings.
@ See README_MODDED.md for a full editing guide.

LittlerootTown_EventScript_CatLeagueNpc::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_COSME_REWARD, CatLeagueReward_EventScript_AlreadyReceived
	goto_if_set FLAG_LEAGUE_WON_CATS_ONLY, CatLeagueReward_EventScript_GiveReward
	goto_if_set FLAG_IS_CHAMPION, CatLeagueReward_EventScript_ChampionNotEligible
	msgbox CatLeagueReward_Text_WaitingForCatTrainer, MSGBOX_NPC
	release
	end

CatLeagueReward_EventScript_ChampionNotEligible::
	msgbox CatLeagueReward_Text_ChampionNotEligible, MSGBOX_NPC
	release
	end

CatLeagueReward_EventScript_GiveReward::
	msgbox CatLeagueReward_Text_EligibleIntro, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_COSME
	specialvar VAR_RESULT, GiveCosmeLeagueReward
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, CatLeagueReward_EventScript_ReceiveCosmeParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, CatLeagueReward_EventScript_ReceiveCosmePC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

CatLeagueReward_EventScript_ReceiveCosmeParty::
	call CatLeagueReward_EventScript_ReceiveCosmeFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CatLeagueReward_EventScript_FinishReward
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto CatLeagueReward_EventScript_FinishReward
	end

CatLeagueReward_EventScript_ReceiveCosmePC::
	call CatLeagueReward_EventScript_ReceiveCosmeFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CatLeagueReward_EventScript_TransferCosmeToPC
	call Common_EventScript_NameReceivedBoxMon
	goto CatLeagueReward_EventScript_TransferCosmeToPC
	end

CatLeagueReward_EventScript_TransferCosmeToPC::
	call Common_EventScript_TransferredToPC
	goto CatLeagueReward_EventScript_FinishReward
	end

CatLeagueReward_EventScript_ReceiveCosmeFanfare::
	bufferspeciesname STR_VAR_2, SPECIES_COSME
	playfanfare MUS_OBTAIN_ITEM
	message CatLeagueReward_Text_ObtainedCosme
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_COSME
	return

CatLeagueReward_EventScript_FinishReward::
	setflag FLAG_RECEIVED_COSME_REWARD
	msgbox CatLeagueReward_Text_AfterReward, MSGBOX_DEFAULT
	release
	end

CatLeagueReward_EventScript_AlreadyReceived::
	msgbox CatLeagueReward_Text_AlreadyReceived, MSGBOX_NPC
	release
	end

CatLeagueReward_Text_WaitingForCatTrainer:
	.string "MOM: Oh, hello, dear!\p"
	.string "I've been waiting for a feline\n"
	.string "TRAINER who would conquer the\l"
	.string "POKéMON LEAGUE.\p"
	.string "Only a team of cats, from start to\n"
	.string "finish, would prove worthy.\p"
	.string "When that day comes, I have a very\n"
	.string "special gift waiting…$"

CatLeagueReward_Text_ChampionNotEligible:
	.string "MOM: So you became CHAMPION!\n"
	.string "I'm so proud of you!\p"
	.string "But the gift I keep is not for any\n"
	.string "champion.\p"
	.string "It belongs to one who entered the\n"
	.string "HALL OF FAME with nothing but\l"
	.string "feline partners at their side.$"

CatLeagueReward_Text_EligibleIntro:
	.string "MOM: Incredible…!\p"
	.string "You conquered the LEAGUE with a team\n"
	.string "of cats, just as I dreamed!\p"
	.string "Please, accept this COSME.\n"
	.string "It has earned its place beside you!$"

CatLeagueReward_Text_ObtainedCosme:
	.string "{PLAYER} received a COSME!$"

CatLeagueReward_Text_AfterReward:
	.string "MOM: Raise that COSME with pride.\n"
	.string "The feline spirit runs in your team!$"

CatLeagueReward_Text_AlreadyReceived:
	.string "MOM: My gift is already in your care.\p"
	.string "May your cats keep prowling\n"
	.string "victoriously across HOENN!$"
