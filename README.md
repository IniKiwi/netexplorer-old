# IniKiwi's net-explorer

licence: gpl-3.0

## use   
`inikiwi-netexplorer [-r <requests>] [-j <threads>] [-t <timeout>] [--hide-fail] [--offline] {explore | search} <ip>[:port]`
### tasks
- `explore` scan ports of a given ip. for each ports prints the requests status:   `FAIL`, `SKIPPED` or `OK`. if status is `OK` the address and port is saved in a database. explore task can be multithreaded.
- `search` search a ip in the database and test it. use `--offline` to skip test.
### arguments
- `port` (optional) can be 
    - a number between 0 and 65535.
    - `random` or `rand`to generate random port.
    - `popular` or `pop` to test only most used ports: 20/21(ftp) 22(ssh) 80(http) and more. (default).
    - example: `192.168.0.*:pop`
- `ip` use `[0-255].[0-255].[0-255].[0-255]` format or `random` to generate random ips (only for explore tasks). example: `192.168.0.*` for local ips.
- `-r <requests>` number of requests to do in a thread of a `explore random` task.
- `-j <threads>` number of threads used for network tasks (explore random). a thread can do one request at a time.
- `-t <timeout>` set timeout for requests. if the timeout is reached the requests is set to `FAIL`.
- `--hide-fail` hides failed requests (timeout). show only requests with `OK` and `SKIPPED` status.
- `--offline` set all network requests to `SKIPPED`. makes search action faster.

## build
### debian based 
```bash
sudo apt-get install build-essential cmake libpthread-stubs0-dev libsqlite3-dev
mkdir build && cd build
cmake ..
make
./inikiwi-netexplorer
```