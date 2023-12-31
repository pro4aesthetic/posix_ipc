#include "sconf.h"

int main(void)
{
    struct sockaddr_un svaddr, claddr;
    int sfd;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);       /* Create server socket */
    if (sfd == -1)
        perror("socket");

    /* Construct well-known address and bind server socket to it */

    /* For an explanation of the following check, see the erratum note for
       page 1168 at http://www.man7.org/tlpi/errata/. */

    if (strlen(SOCK_PATH) > sizeof(svaddr.sun_path) - 1)
    {
        printf("Server socket path too long: %s", SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    if (remove(SOCK_PATH) == -1 && errno != ENOENT)
        perror("remove-/tmp/saddr");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1)
        perror("bind");
    
    /* Receive messages, convert to uppercase, and return to client */

    for (;;) {
        len = sizeof(struct sockaddr_un);
        const auto start_time = Clock::now();

        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                            (struct sockaddr *) &claddr, &len);
        const auto end_time = Clock::now();
        const auto dur = end_time - start_time;

        std::cout << "receive = " << duration_cast<milliseconds>(dur).count() << " ms\n";
        if (numBytes == -1)
            perror("recvfrom");

        printf("Server received %ld bytes from %s\n", (long) numBytes,
                claddr.sun_path);

//        for (j = 0; j < numBytes; j++)
//            buf[j] = toupper((unsigned char) buf[j]);

//        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) !=
//                numBytes)
//            fatal("sendto");
    }
}
