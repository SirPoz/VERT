#include "myqueue.h"

int main(int argc, char *argv[])
{
   message_t msg;  /* message buffer */
   int msgid = -1; /* message queue ID */

   /* Create new message queue */
   if ((msgid = msgget(KEY, PERM | IPC_CREAT | IPC_EXCL)) == -1)
   {
      /* error handling */
      fprintf(stderr, "%s: Error creating message queue\n", argv[0]);
      return EXIT_FAILURE;
   }

   /* Receive messages in an infinite loop */
   while (1)
   {
      if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 0, 0) == -1)
      {
         /* error handling */
         fprintf(stderr, "%s: Can't receive from message queue\n", argv[0]);
         return EXIT_FAILURE;
      }
      printf("Message received: %s\n", msg.mText);
   }
   return EXIT_SUCCESS;
}
