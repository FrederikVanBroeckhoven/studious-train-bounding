cat al.obj | grep "^[v] " | sed "s/v //g" | sed "s/[[:space:]]/\n/g"
