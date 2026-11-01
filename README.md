# COMPSCI 3SH3 — Assignment 3: The Sleeping Teaching Assistant

A solution to the Sleeping TA problem, written with POSIX threads, mutex locks and semaphores.

One TA thread and `n` student threads share an office with one desk and 3 hallway chairs:

- Students alternate between programming and asking the TA for help.
- If a chair is free, the student sits down and wakes the TA with a semaphore. If no chair is free, the student goes back to programming and tries again later.
- The TA naps when nobody is waiting. After helping a student, the TA helps every waiting student in turn before napping again.
- Each student finishes after receiving help 3 times.

## Build and run

```sh
gcc -Wall -pthread A3.c -o A3
./A3 <num_students>        # e.g. ./A3 5
```

Unnamed semaphores (`sem_init`) do not work on macOS, so run the program on Linux. The repo includes a VS Code dev container: install the **Dev Containers** extension, then choose **Reopen in Container**.

## Contributions

### Zeynep

- Set up the project: the GitHub repo and the Linux dev container
- Declared the shared state: the chair counter, the mutex and the semaphores
- Wrote the student thread (`student_thread`) and the `random_sleep` helper
- Wrote the TA thread (`ta_thread`)

### Bakhtawar

-
