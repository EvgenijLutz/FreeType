# Common helpers

echo "hello handsome"

test() {
	echo "test"
}

# Console output formatting
# https://stackoverflow.com/a/2924755
bold=$(tput bold)
normal=$(tput sgr0)


# Checks if the path exists
assert_path() {
  local path=$1
  if [ ! -d "$path" ]; then
    echo "$path does not exist. Check if the path correct and try again."
    exit 1
  fi
}


# Checks if an error happened recently and terminates if it's true
exit_if_error() {
  local result=$?
  if [ $result -ne 0 ] ; then
     echo "Received an exit code $result, aborting"
     exit 1
  fi
}