# #!/bin/bash

# awk '{print $2}' ./$1 | sort -n | uniq -c | sort -k1rn -k2 | awk '{print $2 "\t" $1}'
# # awk '{print $1}' ./$1
# NAMES=($(awk '{print $1}' ./$1))
# printf '%s\n' "${NAMES[@]}" | awk '!($0 in seen){seen[$0];next} 1'