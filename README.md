# ipc

A library that should make it possible to call a function in one process
and invoke the function in another process, possibly on another computer.

## Todo
- [x] Implement receiver protocol
- [x] Implement sender protocol
- [x] Implement transport (raw TCP, udp, url, post-with payload, etc)
- [ ] Generalize sender/receiver protocols (plug and play transport protocol)

## Stretch goals
- [ ] Make function calls that automatically maps to http-post calls
