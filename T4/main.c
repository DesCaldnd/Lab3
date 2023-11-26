//
// Created by Des Caldnd on 11/23/2023.
//
#include "../structs.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

enum command_type
{ CREATE, REMOVE, SEARCH, RECIEVE, ALL_RECIEVED, ALL_EXPIRED, PRINT_ALL, QUIT, UNDEF };

struct Address
{
    struct String city, street, building;
    int house_num, flat_num;
    char reciever_ind[7];
};

struct Mail
{
    struct Address address;
    double weight;
    char post_id[15];
    char creation_time[20];
    char recieve_time[20];
    int recieved;
};

VECTOR(Mail, struct Mail)

struct Post
{
    struct Address* post_address;
    struct vector_Mail mails;
};


enum command_type get_command_type(char* str);
int configure_address(struct Address* address);
void increment_post_id(char* id);
void create_mail(struct Post* post, char* id);
void remove_mail(struct Post* post);
void search_mail(struct Post* post);
void recieve_mail(struct Post* post);
void all_recieved(struct Post* post);
void all_expired(struct Post* post);
void print_mail(struct Mail* mail, void*);
void print_address(struct Address address);
int is_needed_sym(char sym);

int mail_eq_id(struct Mail lhs, struct Mail rhs);
void delete_mail(struct Mail* mail, void*);


int main()
{
    struct Post post;
    post.mails = init_vector_Mail(10);
    char post_id[] = "aaaaaaaaaaaaaa";

    if(!is_valid_vector_Mail(&post.mails))
    {
        printf("Bad alloc\n");
        return 0;
    }

    struct Address post_address;
    int status = configure_address(&post_address);
    if (!status)
    {
        destroy_vector_Mail(&post.mails);
        printf("Bad alloc\n");
        return 0;
    }

    post.post_address = &post_address;

    char prompt[15];
    printf("\nEnter command: ");
    scanf("%14s", prompt);
    enum command_type type = get_command_type(prompt);

    while (type != QUIT)
    {
        switch (type)
        {
            case CREATE:
            {
                create_mail(&post, post_id);
                break;
            }
            case REMOVE:
            {
                remove_mail(&post);
                break;
            }
            case SEARCH:
            {
                search_mail(&post);
                break;
            }
            case RECIEVE:
            {
                recieve_mail(&post);
                break;
            }
            case ALL_RECIEVED:
            {
                all_recieved(&post);
                break;
            }
            case ALL_EXPIRED:
            {
                all_expired(&post);
                break;
            }
            case PRINT_ALL:
            {
                print_address(*post.post_address);
                printf("\nNumber of mails: %zu\n\n", size_vector_Mail(&post.mails));
                for_all_vector_Mail(&post.mails, &print_mail, NULL);
                printf("\n\n");
                break;
            }
            case UNDEF:
            {
                printf("This command is wrong. You can enter following comands:\n\t-create\n\t-remove\n\t-search\n\t-recieve\n\t-all_recieved\n\t-all_expired\n\t-quit\n");
                break;
            }
        }
        if (!is_valid_vector_Mail(&post.mails))
        {
            printf("Bad alloc\n");
            return 0;
        }
        printf("\nEnter command: ");
        scanf("%14s", prompt);
        type = get_command_type(prompt);
    }

    for_all_vector_Mail(&post.mails, &delete_mail, NULL);
    destroy_vector_Mail(&post.mails);
    return 0;
}

enum command_type get_command_type(char* str)
{
    if (strcmp(str, "create") == 0)
        return CREATE;
    else if (strcmp(str, "remove") == 0)
        return REMOVE;
    else if (strcmp(str, "search") == 0)
        return SEARCH;
    else if (strcmp(str, "recieve") == 0)
        return RECIEVE;
    else if (strcmp(str, "all_recieved") == 0)
        return ALL_RECIEVED;
    else if (strcmp(str, "all_expired") == 0)
        return ALL_EXPIRED;
    else if (strcmp(str, "print_all") == 0)
        return PRINT_ALL;
    else if (strcmp(str, "quit") == 0)
        return QUIT;
    else
        return UNDEF;
}

int configure_address(struct Address* address)
{
    printf("Enter city: ");
    address->city = init_string_from_stream(stdin, &isalpha);
    printf("Enter street: ");
    address->street = init_string_from_stream(stdin, &isalpha);
    printf("Enter house number: ");
    int counter = scanf("%d", &address->house_num);
    int is_building = 0;
    printf("Is building required: ");
    scanf("%d", &is_building);
    if(is_building)
    {
        printf("Enter building: ");
        address->building = init_string_from_stream(stdin, &isalnum);
    } else
    {
        address->building = init_string("");
    }
    printf("Enter flat number: ");
    counter += scanf("%d", &address->flat_num);
    printf("Enter reciever index (6 char): ");
    counter += scanf("%6s", address->reciever_ind);

    if (!is_valid_string(&address->city) || address->city.size < 1 || !is_valid_string(&address->street) || address->street.size < 1 ||
            !is_valid_string(&address->building) || counter < 3)
    {
        destroy_string(&address->city);
        destroy_string(&address->street);
        destroy_string(&address->building);
        return 0;
    }
    return 1;
}


void increment_post_id(char* id)
{
    int need_new_op = 1, ind = 13;
    while (need_new_op && ind > 0)
    {
        need_new_op = 0;
        if (id[ind] == '9')
        {
            need_new_op = 1;
            id[ind] = 'a';
        } else if (id[ind] == 'z')
        {
            id[ind] = 'A';
        } else if (id[ind] == 'Z')
        {
            id[ind] = '0';
        } else
        {
            ++id[ind];
        }

        --ind;
    }
}

void create_mail(struct Post* post, char* id)
{
    struct Mail mail;
    if (!configure_address(&mail.address))
    {
        printf("Address is incorrect ore no memory\n");
        return;
    }
    int counter = 0;
    int day, month, year, hour, minute, second;
    printf("Enter weight: ");
    counter = scanf("%lf", &mail.weight);
    if (counter != 1)
    {
        printf("Incorrect weight\n");
        return;
    }
    printf("Enter recieve time: ");
    counter = scanf("%2d:%2d:%4d %2d:%2d:%2d", &day, &month, &year, &hour, &minute, &second);
    if (counter != 6 || day < 0 || month < 0 || hour < 0 || minute < 0 || second < 0 || day > 31 || month > 12 || hour > 24 || minute > 60 || second > 60)
    {
        printf("Incorrect time\n");
        return;
    }

    time_t cur_tume = time(NULL);

    struct tm *current = localtime(&cur_tume);

    sprintf(mail.creation_time, "%2d:%2d:%4d %2d:%2d:%2d", current->tm_mday, current->tm_mon, current->tm_year + 1900, current->tm_hour, current->tm_min, current->tm_sec);
    sprintf(mail.recieve_time, "%2d:%2d:%4d %2d:%2d:%2d", day, month, year, hour, minute, second);
    mail.recieved = 0;
    strcpy(mail.post_id, id);
    increment_post_id(id);

    push_vector_Mail(&post->mails, mail, &delete_mail);
    if (is_valid_vector_Mail(&post->mails))
    {
        printf("Successfully created! Mail id is \"%14s\"\n", mail.post_id);
    } else
    {
        printf("Error while creating!\n");
    }
}

void remove_mail(struct Post* post)
{
    printf("Enter mail id: ");
    char tmp_id[15];
    scanf("%14s", tmp_id);

    struct Mail tmp;
    strcpy(tmp.post_id, tmp_id);

    struct Mail* mail = find_vector_Mail(&post->mails, tmp, &mail_eq_id);

    if (mail == NULL)
    {
        printf("No such mail\n");
        return;
    }
    remove_vector_Mail(&post->mails, mail - post->mails.data);
    printf("Removed successfully\n");
}

void search_mail(struct Post* post)
{
    printf("Enter mail id: ");
    char tmp_id[15];
    scanf("%14s", tmp_id);

    struct Mail tmp;
    strcpy(tmp.post_id, tmp_id);

    struct Mail* mail = find_vector_Mail(&post->mails, tmp, &mail_eq_id);

    if (mail == NULL)
    {
        printf("No such mail\n");
        return;
    }

    print_mail(mail, NULL);
}

void recieve_mail(struct Post* post)
{
    printf("Enter mail id: ");
    char tmp_id[15];
    scanf("%14s", tmp_id);

    struct Mail tmp;
    strcpy(tmp.post_id, tmp_id);

    struct Mail* mail = find_vector_Mail(&post->mails, tmp, &mail_eq_id);

    if (mail == NULL)
    {
        printf("No such mail\n");
        return;
    }

    if(mail->recieved != 0)
        printf("It is already recieved\n");
    else
    {
        mail->recieved = 1;
        printf("Mail recieved successfully\n");
    }
}

int is_recieved(struct Mail* mail)
{
    return mail->recieved;
}

int is_expired(struct Mail* mail)
{
    time_t cur_tume = time(NULL);
    int day, month, year, hour, minute, second;
    sscanf(mail->recieve_time, "%2d:%2d:%4d %2d:%2d:%2d", &day, &month, &year, &hour, &minute, &second);

    struct tm _time;
    _time.tm_mday = day;
    _time.tm_mon = month;
    _time.tm_year = year - 1900;
    _time.tm_hour = hour;
    _time.tm_min = minute;
    _time.tm_sec = second;

    time_t rec_time = mktime(&_time);

    if (cur_tume > rec_time && !mail->recieved)
        return 1;
    else return 0;
}

void print_mail_if(struct Mail* mail, void* data)
{
    int (*pred)(struct Mail*) = data;
    if (pred(mail))
    {
        print_mail(mail, NULL);
    }
}

void all_recieved(struct Post* post)
{
    for_all_vector_Mail(&post->mails, &print_mail_if, &is_recieved);
}

void all_expired(struct Post* post)
{
    for_all_vector_Mail(&post->mails, &print_mail_if, &is_expired);
}

void print_mail(struct Mail* mail, void*)
{
    printf("Address data:\n");
    print_address(mail->address);
    printf("Weight: %.3lf kg\nPost id: %s\nCreation time: %s\nRecieve time: %s\nRecieved: %s\n\n", mail->weight,
           mail->post_id, mail->creation_time, mail->recieve_time, mail->recieved == 0 ? "false" : "true");
}

void print_address(struct Address address)
{
    if (address.building.size > 0)
    printf("City: %s\nStreet: %s\nHouse number: %d\nBuilding: %s\nFlat number: %d\nReciever index: %s\n", address.city.data,
           address.street.data, address.house_num, address.building.data, address.flat_num, address.reciever_ind);
    else
        printf("City: %s\nStreet: %s\nHouse number: %d\nFlat number: %d\nReciever index: %s\n", address.city.data,
               address.street.data, address.house_num, address.flat_num, address.reciever_ind);
}

int mail_eq_id(struct Mail lhs, struct Mail rhs)
{
    return strcmp(lhs.post_id, rhs.post_id) == 0;
}

void delete_mail(struct Mail* mail, void*)
{
    destroy_string(&mail->address.building);
    destroy_string(&mail->address.city);
    destroy_string(&mail->address.street);
}