@ Cat Living Dex Challenge: NPC reward flow and editable dialogue strings.

LittlerootTown_EventScript_CatLivingDexNpc::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TITA_REWARD, CatLivingDexReward_EventScript_AlreadyReceived
	specialvar VAR_RESULT, CountPlayerCatLivingDexSpecies
	goto_if_eq VAR_RESULT, 6, CatLivingDexReward_EventScript_GiveReward
	buffernumberstring STR_VAR_1, VAR_RESULT
	msgbox CatLivingDexReward_Text_InProgress, MSGBOX_NPC
	release
	end

CatLivingDexReward_EventScript_GiveReward::
	msgbox CatLivingDexReward_Text_EligibleIntro, MSGBOX_DEFAULT
	closemessage
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_TITA
	specialvar VAR_RESULT, GiveTitaLivingDexReward
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, CatLivingDexReward_EventScript_ReceiveTitaParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, CatLivingDexReward_EventScript_ReceiveTitaPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

CatLivingDexReward_EventScript_ReceiveTitaParty::
	call CatLivingDexReward_EventScript_ReceiveTitaFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CatLivingDexReward_EventScript_FinishReward
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto CatLivingDexReward_EventScript_FinishReward
	end

CatLivingDexReward_EventScript_ReceiveTitaPC::
	call CatLivingDexReward_EventScript_ReceiveTitaFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CatLivingDexReward_EventScript_TransferTitaToPC
	call Common_EventScript_NameReceivedBoxMon
	goto CatLivingDexReward_EventScript_TransferTitaToPC
	end

CatLivingDexReward_EventScript_TransferTitaToPC::
	call Common_EventScript_TransferredToPC
	goto CatLivingDexReward_EventScript_FinishReward
	end

CatLivingDexReward_EventScript_ReceiveTitaFanfare::
	bufferspeciesname STR_VAR_2, SPECIES_TITA
	playfanfare MUS_OBTAIN_ITEM
	message CatLivingDexReward_Text_ObtainedTita
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_TITA
	return

CatLivingDexReward_EventScript_FinishReward::
	setflag FLAG_RECEIVED_TITA_REWARD
	msgbox CatLivingDexReward_Text_AfterReward, MSGBOX_DEFAULT
	release
	end

CatLivingDexReward_EventScript_AlreadyReceived::
	msgbox CatLivingDexReward_Text_AlreadyReceived, MSGBOX_NPC
	release
	end

CatLivingDexReward_Text_InProgress:
	.string "Ah, a TRAINER with an eye for cats!\p"
	.string "I'm cataloguing every feline native\n"
	.string "to HOENN.\p"
	.string "Bring me one of each: MEOWTH, PERSIAN,\n"
	.string "SKITTY, DELCATTY, ZANGOOSE, and ABSOL.\p"
	.string "They must be yours alone, caught or\n"
	.string "raised by you. No trades.\p"
	.string "You currently have {STR_VAR_1} of\n"
	.string "those six feline companions...$"

CatLivingDexReward_Text_EligibleIntro:
	.string "Marvelous...!\p"
	.string "You have gathered every wild cat of\n"
	.string "HOENN, each one truly your own!\p"
	.string "COSME rewards those who conquer with\n"
	.string "fury. TITA belongs to those who know\l"
	.string "them all.\p"
	.string "Please, accept this TITA.\n"
	.string "She has waited patiently for a\l"
	.string "collector like you!$"

CatLivingDexReward_Text_ObtainedTita:
	.string "{PLAYER} received a TITA!$"

CatLivingDexReward_Text_AfterReward:
	.string "Care for that TITA gently.\n"
	.string "She prefers a warm nap after a job\l"
	.string "well done!$"

CatLivingDexReward_Text_AlreadyReceived:
	.string "My TITA is already with you.\p"
	.string "Keep building your bond with every\n"
	.string "cat in HOENN!$"
