cmd_/media/sf_Shared/linux_backdoor/modules.order := {   echo /media/sf_Shared/linux_backdoor/keylogger.ko; :; } | awk '!x[$$0]++' - > /media/sf_Shared/linux_backdoor/modules.order
