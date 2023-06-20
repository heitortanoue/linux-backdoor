cmd_/media/sf_Shared/linux_backdoor/keylogger.mod := printf '%s\n'   keylogger.o | awk '!x[$$0]++ { print("/media/sf_Shared/linux_backdoor/"$$0) }' > /media/sf_Shared/linux_backdoor/keylogger.mod
