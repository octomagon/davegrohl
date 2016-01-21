#!/bin/bash

APP="$(dirname $0)/../dave"
ERRORS=false

passed(){
	printf "passed.\n"
}

failed(){
	printf "\033[31mfailed.\e[0m\n"
	ERRORS=true
}


# Make sure dave exists.
if [ ! -f $APP ]; then
    echo "I can't find dave. Run 'make' to create him."
    exit
fi

# Test hashing functions.
printf "Testing good password - Windows SMB... "
$APP -H 12cc588f8eabee46d28f6146d3aa0c78 -p baculum > /dev/null && passed || failed

printf "Testing bad password - Windows SMB... "
$APP -H 12cc588f8eabee46d28f6146d3aa0c78 -p vasdeferens > /dev/null && failed || passed

printf "Testing good password - PBKDF2 (Salted SHA512)... "
$APP -H 1f592250de5c494ad7440e544b5624d5808e9824243e067017c25fbb6a707aea93d7ddc842cad8c8916ab913750ba2c9b954441c151cdc16563c2bf48b19d3f12a74a89d51ff3031b2273ac98390692ee2e5b0bb793c23302784794cdde118eff3e676d49804e7516d6a23a0ce4e1538b91d5fadadefe917dde1d2558f85c8cb -S bed3c67d617de12781f2a17addab51719bce144ccf86ede75eeb1ce9bc13c952 -R 45662 -p iloveyou > /dev/null && passed || failed

printf "Testing bad password - PBKDF2 (Salted SHA512)... "
$APP -H 1f592250de5c494ad7440e544b5624d5808e9824243e067017c25fbb6a707aea93d7ddc842cad8c8916ab913750ba2c9b954441c151cdc16563c2bf48b19d3f12a74a89d51ff3031b2273ac98390692ee2e5b0bb793c23302784794cdde118eff3e676d49804e7516d6a23a0ce4e1538b91d5fadadefe917dde1d2558f85c8cb -S bed3c67d617de12781f2a17addab51719bce144ccf86ede75eeb1ce9bc13c952 -R 45662 -p boobies > /dev/null && failed || passed


if $ERRORS; then
	printf "\n\033[31mTesting found errors :(\e[0m\n"
else
	printf "\n\e[4mAll tests passed\e[0m\n\n"
fi
