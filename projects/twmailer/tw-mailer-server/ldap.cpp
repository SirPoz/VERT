#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ldap.h>
#include <string>
#include <iostream>
#include <sstream>


#define LDAP_URI "ldap://ldap.technikum-wien.at:389"
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER ""

/* bind credentials
 * TODO: replace placeholders "if21b999" and "123456" with your credentials
 *     ... DO NOT SHARE YOUR CREDENTIALS !!
 */
#define BIND_USER ""	
#define BIND_PW ""

#define LDAP_LOGIN_ERROR 0
#define LDAP_LOGIN_FAILED 2
#define LDAP_LOGIN_SUCCESS 1

/* anonymous bind with user and pw empty  
#define BIND_USER ""
#define BIND_PW ""
*/

using namespace std;


class Ldaphandler{

public:

int login(string username, string password){
   printf("user:%s pass:%s\n",username.c_str(),password.c_str());

   LDAP *ld;			/* LDAP resource handle */
   LDAPMessage *result, *e;	/* LDAP result handle */
   BerElement *ber;		/* array of attributes */
   char *attribute;
   BerValue **vals;
   string tempfilter = "(uid=" +  username + ")";
   BerValue *servercredp;
   BerValue cred;
   cred.bv_val = (char *)password.c_str();
   cred.bv_len=strlen(password.c_str());
   int i,rc=0;
   string tempdn =  "uid="+username+",ou=people,dc=technikum-wien,dc=at";
  // char *dntemp =  const_cast<char *> (tempdn.c_str());


   const char *filter = ("(uid=" +  username + ")").c_str();
   const char *attribs[] = { "uid", "cn", NULL };		/* attribute array for search */

   int ldapversion = LDAP_VERSION3;

   /* setup LDAP connection */
   if (ldap_initialize(&ld,LDAP_URI) != LDAP_SUCCESS)
   {
      fprintf(stderr,"ldap_init failed");
       return LDAP_LOGIN_ERROR;
   }

   printf("connected to LDAP server %s\n",LDAP_URI);

   if ((rc = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ldapversion)) != LDAP_SUCCESS)
   {
      fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n", ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
      return LDAP_LOGIN_ERROR;
   }

   if ((rc = ldap_start_tls_s(ld, NULL, NULL)) != LDAP_SUCCESS)
   {
      fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
       return LDAP_LOGIN_ERROR;
   }

   /* anonymous bind */
   rc = ldap_sasl_bind_s(ld,tempdn.c_str(),LDAP_SASL_SIMPLE,&cred,NULL,NULL,&servercredp);

   if (rc != LDAP_SUCCESS)
   {
      fprintf(stderr,"LDAP bind error: %s\n",ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
      return LDAP_LOGIN_FAILED;
   }
   else
   {
      printf("bind successful\n");
   }

   /* perform ldap search */
   rc = ldap_search_ext_s(ld, SEARCHBASE, SCOPE, filter, (char **)attribs, 0, NULL, NULL, NULL, 500, &result);

   if (rc != LDAP_SUCCESS)
   {
      fprintf(stderr,"LDAP search error: %s\n",ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
       return LDAP_LOGIN_FAILED;
   }

   printf("Total results: %d\n", ldap_count_entries(ld, result));

   for (e = ldap_first_entry(ld, result); e != NULL; e = ldap_next_entry(ld,e))
   {
      printf("DN: %s\n", ldap_get_dn(ld,e));
      
      /* Now print the attributes and values of each found entry */

      for (attribute = ldap_first_attribute(ld,e,&ber); attribute!=NULL; attribute = ldap_next_attribute(ld,e,ber))
      {
         if ((vals=ldap_get_values_len(ld,e,attribute)) != NULL)
         {
            for (i=0;i < ldap_count_values_len(vals);i++)
            {
               printf("\t%s: %s\n",attribute,vals[i]->bv_val);
            }
            ldap_value_free_len(vals);
         }
         /* free memory used to store the attribute */
         ldap_memfree(attribute);
      }
      /* free memory used to store the value structure */
      if (ber != NULL) ber_free(ber,0);

      printf("\n");
   }
  
   /* free memory used for result */
   ldap_msgfree(result);
   printf("LDAP search suceeded\n");
  
   ldap_unbind_ext_s(ld, NULL, NULL);
   return LDAP_LOGIN_SUCCESS;


}
};