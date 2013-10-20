#-----------------------------------------------------------------------
#	Functions for pwlibs tests
#-----------------------------------------------------------------------
pwl_start() {
    pwl_stub="/tmp/pwl.$$"
    trap pwl_clean EXIT
    pwl_rc=0
    if [ -f "$HOME/.pitile" ]; then
	if [ ! -f "$HOME/.pitile.tsave" ]; then
	    mv "$HOME/.pitile" "$HOME/.pitile.tsave"
	fi
	rm -f "$HOME/.pitile"
    fi
    if [ -f "$HOME/.piwall" ]; then
	if [ ! -f "$HOME/.piwall.tsave" ]; then
	    mv "$HOME/.piwall" "$HOME/.piwall.tsave"
	fi
	rm -f "$HOME/.piwall"
    fi
    LD_LIBRARY_PATH=../src/.libs:${LD_LIBRARY_PATH}
    export LD_LIBRARY_PATH
}

pwl_clean() {
    if [ -n "$pwl_stub" ]; then
	rm -f "$pwl_stub".*
    fi
}

pwl_end() {
    if [ -f "$HOME/.pitile.tsave" ]; then
	mv "$HOME/.pitile.tsave" "$HOME/.pitile"
    fi
    if [ -f "$HOME/.piwall.tsave" ]; then
	mv "$HOME/.piwall.tsave" "$HOME/.piwall"
    fi
    return $pwl_rc
}

pwl_pitile() {
    cat > "$HOME/.pitile"
}
pwl_piwall() {
    cat > "$HOME/.piwall"
}

pwl_run() {
    _out="$pwl_stub.out"
    _err="$pwl_stub.err"
    pwl_cmd="$@"
    "$@" > "$_out" 2> "$_err"
    _rc=$?
    (
	if [ "$_rc" -ne 0 ]; then
	    echo "== rc =="
	    echo $_rc
	fi
	if [ -s "$_out" ]; then
	    echo "== out =="
	    cat "$_out"
	fi
	if [ -s "$_err" ]; then
	    echo "== err =="
	    cat "$_err"
	fi
	echo "== end =="
    ) > "$pwl_stub.full"
}

pwl_expect() {
    cat > "$pwl_stub.exp"
    echo "== end ==" >> "$pwl_stub.exp"
    diff -u "$pwl_stub.exp" "$pwl_stub.full" > "$pwl_stub.dif"
    if [ $? -ne 0 ]; then
	echo "*** $pwl_cmd"
	cat "$pwl_stub.dif"
	pwl_rc=1
    fi
}
