exit 0

echo '{"test": [1,2,{"a":3}]}' | i --load j

i json_import.i.helper
exit $?
