# Lab Work #7: Linux Shell Scripting

## Contents
- [Objective](#objective)
- [Task Description](#task-description)
- [Script Implementation](#script-implementation)
- [Execution Results](#execution-results)
- [Conclusion](#conclusion)

## Objective
Develop a Linux shell script to:
- Check if a user exists in the system
- Display the user's UID
- Implement parameter validation and error handling

## Task Description
**Variant 6 Requirements:**
1. Verify if a user exists via `/etc/passwd`
2. Output the user's UID if found
3. Validate input parameters
4. Suppress unnecessary system messages

## Script Implementation
```bash
#!/bin/bash
# Author: Dudolin Denis, UIS-312
# Variant: 6
# Task: Check user existence and display UID using /etc/passwd

# Check if username argument is provided
if [ $# -lt 1 ]; then
    echo -e "\033[31mError:\033[0m Username argument required!"
    echo -e "Usage: $0 <username>"
    exit 1
fi

# Verify user existence
if ! grep -q "^$1:" /etc/passwd; then
    echo -e "\033[31mError:\033[0m User \033[31m$1\033[0m doesn't exist!"
    exit 1
fi

# Extract and display UID
uid=$(grep "^$1:" /etc/passwd | cut -d: -f3)
echo -e "User \033[32m$1\033[0m UID: $uid"
exit 0
```

## Execution Results
```
dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh 
Ошибка: Необходимо передать имя пользователя!
Пример: ./cu.sh <username>

dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh Oleg
Ошибка: Пользователь Oleg не существует!

dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh root
UID пользователя root: 0

dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh mail
UID пользователя mail: 8

dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh _apt
UID пользователя _apt: 105

dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh news
UID пользователя news: 9

dudolin-denis@dudolin-denis-uis312:~$ ./cu_dudolin.sh gdm
UID пользователя gdm: 128

```

## Conclusion
- Developed a Bash script to check user existence in `/etc/passwd`
- Implemented argument validation and error handling
- Successfully outputs UID for valid users
- Ready for system administration use