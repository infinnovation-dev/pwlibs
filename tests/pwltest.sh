#-----------------------------------------------------------------------
#	Functions for pwlibs tests
#-----------------------------------------------------------------------
pwl_start() {
    PWLDIR=`pwd`
    HOME="$PWLDIR"
    export HOME
    pwl_stub="/tmp/pwl.$$"
    trap pwl_clean EXIT
    pwl_rc=0
    rm -f "$PWLDIR/.pitile" "$PWLDIR/.piwall"
    LD_LIBRARY_PATH=../src/.libs:${LD_LIBRARY_PATH}
    export LD_LIBRARY_PATH
}

pwl_clean() {
    if [ -n "$pwl_stub" ]; then
	rm -f "$pwl_stub".*
    fi
}

pwl_end() {
    return $pwl_rc
}

pwl_pitile() {
    cat > "$PWLDIR/.pitile"
}
pwl_piwall() {
    cat > "$PWLDIR/.piwall"
}

pwl_run() {
    _out="$pwl_stub.out"
    _err="$pwl_stub.err"
    pwl_cmd="$@"
    $PWL_RUN "$@" > "$_out" 2> "$_err"
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
