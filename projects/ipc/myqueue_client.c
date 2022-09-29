#include "myqueue.h"

int main(int argc, char *argv[])
{
   message_t msg;  /* message buffer */
   int msgid = -1; /* message queue ID */

   /* Argument handling */
   if (argc != 2)
   {
      fprintf(stderr, "Usage: %s <Message>\n", argv[0]);
      return EXIT_FAILURE;
   }

   /* Open message queue */
   if ((msgid = msgget(KEY, PERM)) == -1)
   {
      /* error handling */
      fprintf(stderr, "%s: Can't access message queue\n", argv[0]);
      return EXIT_FAILURE;
   }

   /* Send message */
   msg.mType = 1;
   strncpy(msg.mText, argv[1], MAX_DATA);
   if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1)
   {
      /* error handling */
      fprintf(stderr, "%s: Can't send message\n", argv[0]);
      return EXIT_FAILURE;
   }
   printf("Message sent: %s\n", msg.mText);
   return EXIT_SUCCESS;
}
