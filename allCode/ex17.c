#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#if 0
#define MAX_DATA 512
#define MAX_ROWS 100

#else
#define DYNAMIC_VER 1

#endif

#ifndef DYNAMIC_VER

struct Address {
    int id;
    int set;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

struct Database {
    struct Address rows[MAX_ROWS];
};
#else
struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Database {
    struct Address *rows;
    int max_rows;
    int max_data;
};
#endif

struct Connection {
    FILE *file;
    struct Database *db;
};

/* die: report error message and abort current process */
void die(const char *message, struct Connection *conn)
{
    void Database_close(struct Connection *);
    if (errno) {                // What is errno?
        perror(message);        // What is perror?
    } else {                    // Just printing error message!
        printf("ERROR: %s\n", message);
    }
    Database_close(conn);
    exit(1);   // Need to search exit function to find out how it behaves exactly
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn)
{
    // fread: make point conn->db to the newly read data from
    // conn->file. The data size is sizeof(struct Database) and
    // The number of object to be read is 1.
    // The return value is number of objects read.
    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);

    // The number of object read is different from what supposed to be read,
    // die with following error message.
    if (rc != 1)
        die("Failed to load database.", conn);

    #ifdef DYNAMIC_VER
    conn->db->rows = malloc(sizeof(struct Address) * conn->db->max_rows);

    if (!conn->db->rows)
        die("Memory error", conn);

    rc = fread(conn->db->rows, sizeof(struct Address), conn->db->max_rows, conn->file);

    if (rc != conn->db->max_rows)
        die("Failed to load Addresses.", conn);

    for (int i = 0; i < conn->db->max_rows; ++i) {
        conn->db->rows[i].name = malloc(sizeof(char) * conn->db->max_data);

        if (!conn->db->rows[i].name)
            die("Memory error", conn);

        rc = fread(conn->db->rows[i].name, sizeof(char), conn->db->max_data, conn->file);

        if (rc != conn->db->max_data)
            die("Failed to load Address name.", conn);

        conn->db->rows[i].email = malloc(sizeof(char) * conn->db->max_data);

        if (!conn->db->rows[i].email)
            die("Memory error", conn);

        rc = fread(conn->db->rows[i].email, sizeof(char), conn->db->max_data, conn->file);

        if (rc != conn->db->max_data)
            die("Failed to load Address email.", conn);
    }
    #endif
}

/* Database_open: Accessor & constructor Database positioned in hardware */
struct Connection *Database_open(const char *filename, char mode)
{
    // Dynamically allocate Connection data structure
    struct Connection *conn = malloc(sizeof(struct Connection));

    // if there is no space to allocate above object -- malloc return NULL as error
    if (!conn)
        die("Memory error", conn);

    // Same goes with Database structure
    conn->db = malloc(sizeof(struct Database));

    if (!conn->db)
        die("Memory error", conn);

    if (mode == 'c') { // guess: 'c' for creation?
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+"); // What is r+ means?
                                // From the man fopen, r+ means open for reading and writing

        if (conn->file) { // If it is openable, load the database
            Database_load(conn);
        }
    }

    // By using guard clause + unified error reporting procedure,
    // The whole module got coherent structure.
    if (!conn->file) // file is null pointer -- error occured in fopen
        // errno should be set by the fopen by the documentation.
        // ↑ yet I don't know what the errno is.
        die("Failed to open the file", conn);

    return conn;
}

void Database_close(struct Connection *conn)
{
    if (conn) {                 // There exists opened Connection?
        if (conn->file)         // There exists opened file?
            fclose(conn->file);
        if (conn->db) {         // There exists loaded Database?
#ifdef DYNAMIC_VER
            if (conn->db->rows) {
                for (int i = 0; i < conn->db->max_rows; ++i) {
                    if (conn->db->rows[i].name)
                        free(conn->db->rows[i].name);
                    if (conn->db->rows[i].email)
                        free(conn->db->rows[i].email);
                }
                free(conn->db->rows);
            }
#endif
            free(conn->db);
        }
        free(conn);    // Do the counterpart process in reverse order of opening
    }
}

void Database_write(struct Connection *conn) // counterpart of Database_load
{
    rewind(conn->file); // What is rewind procedure? It should process file pointer
    // ↑ I can guess that it would `rewind` the advanced file pointer address
    // done by calling fread procedure.
    // Yep, it turns out reset the file pointer to the beginning of the file

    // Write conn->db data to the address pointed by conn->file
    int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
    if (rc != 1)
        die("Failed to write database.", conn);

#ifdef DYNAMIC_VER
    rc = fwrite(conn->db->rows, sizeof(struct Address), conn->db->max_rows, conn->file);

    if (rc != conn->db->max_rows)
        die("Failed to write Addresses.", conn);

    for (int i = 0; i < conn->db->max_rows; ++i) {
        rc = fwrite(conn->db->rows[i].name, sizeof(char), conn->db->max_data, conn->file);

        if (rc != conn->db->max_data)
            die("Failed to write Address name.", conn);

        rc = fwrite(conn->db->rows[i].email, sizeof(char), conn->db->max_data, conn->file);

        if (rc != conn->db->max_data)
            die("Failed to write Address email.", conn);
    }
#endif

    // Why we need to flush the file? Is it buffered?
    // We need to reference K&R for this.
    // Since fwrite would depend on put under the hood,
    // there is buffer to given file pointer opened for write
    rc = fflush(conn->file);
    if (rc == -1) // The interface defined in documentation of fflush
        die("Cannot flush database.", conn);
}

/* Database_create: after creation in the hardware file via Database_open,
   set the contents of database as default values. */
#ifndef DYNAMIC_VER
void Database_create(struct Connection *conn)
{
    int i = 0;

    for (i = 0; i < MAX_ROWS; i++) {
        // make a prototype to initialize it
        struct Address addr = {.id = i, .set = 0}; // This is stacked up in this function
        // then just assign it
        conn->db->rows[i] = addr;
// ↑ exploit the fact that structure can be copied around
// -- compiler do the right things for us.
    }
}
#else
void Database_create(struct Connection *conn, int max_rows, int max_data)
{
    conn->db->rows = malloc(sizeof(struct Address) * max_rows);

    if (!conn->db->rows)
        die("Memory error", conn);

    for (int i = 0; i < max_rows; ++i) {
        conn->db->rows[i].name = malloc (sizeof(char) * max_data);

        if (!conn->db->rows[i].name)
            die("Memory error", conn);

        conn->db->rows[i].email = malloc(sizeof(char) * max_data);

        if (!conn->db->rows[i].email)
            die("Memory error", conn);

        conn->db->rows[i].id = i;
        conn->db->rows[i].set = 0;
    }
    conn->db->max_rows = max_rows;
    conn->db->max_data = max_data;
}
#endif

void Database_set(struct Connection *conn, int id, const char *name,
                  const char *email)
{
    struct Address *addr = &conn->db->rows[id]; // Load address
    if (addr->set) // Already set
        die("Already set, delete it first", conn);

    addr->set = 1; // Mark this address entity has been set already
    // WARNING: bug, read the "How To Break It" and fix this
#ifndef DYNAMIC_VER
    char *res = strncpy(addr->name, name, MAX_DATA);
#else
    char *res = strncpy(addr->name, name, conn->db->max_data);
#endif
    // demonstrate the strncpy bug

    // By contraction, strncpy does not return null pointer
    // when it encountered with overflow; it just return the pointer to
    // just past end of the given bound.
    // So I guess the following guiding clause do not work as expected since
    // it does not agree with the contraction from the documentation.
    if (!res)
        die("Name copy failed", conn);
#ifndef DYNAMIC_VER
    res[MAX_DATA - 1] = '\0';
#else
    res[conn->db->max_data - 1] = '\0';
#endif

    // To break these guide clause, all we need to do is give the argument
    // over the MAX_DATA bytes (characters).
#ifndef DYNAMIC_VER
    res = strncpy(addr->email, email, MAX_DATA);
#else
    res = strncpy(addr->email, email, conn->db->max_data);
#endif
    if (!res)
        die("Email copy failed", conn);
#ifndef DYNAMIC_VER
    res[MAX_DATA - 1] = '\0';
#else
    res[conn->db->max_data - 1] = '\0';
#endif
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = &conn->db->rows[id]; // Load the address entity

    if (addr->set) { // if it is set
        Address_print(addr);
    } else {
        die("ID is not set", conn);
    }
}

void Database_delete(struct Connection *conn, int id)
{
    conn->db->rows[id].set = 0; // Just initialize given id
}

void Database_list(struct Connection *conn)
{
    int i = 0;
    struct Database *db = conn->db;

    for (i = 0;
#ifndef DYNAMIC_VER
         i < MAX_ROWS;
#else
         i < conn->db->max_rows;
#endif
         i++) {
        struct Address *cur = &db->rows[i]; // Load current address into the cur

        if (cur->set) { // if current address entity is set
            Address_print(cur);
        }
    }
}

void Database_find(struct Connection *conn, const char *name)
{
    int i = 0;
    struct Database *db = conn->db;
    for (i = 0;
#ifndef DYNAMIC_VER
         i < MAX_ROWS;
#else
         i < conn->db->max_rows;
#endif
         i++) {
        struct Address *cur = &db->rows[i];

        if (strcmp(name, cur->name) == 0) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        die("USAGE: ex17 <dbfile> <action> [action params]", NULL);

    char *filename = argv[1];
    char action = argv[2][0];   // action is a character
    struct Connection *conn = Database_open(filename, action);
    int id = 0;

    if (argc > 3) id = atoi(argv[3]); // first action param is id

#ifndef DYNAMIC_VER
    if (id >= MAX_ROWS) die("There's not that many records.", conn);
#endif

    switch(action) {
        case 'c':
#ifdef DYNAMIC_VER
            if (argc != 5)
                die("Need max rows for addresses, max data for name and email", conn);
            Database_create(conn, atoi(argv[3]), atoi(argv[4]));
#else
            Database_create(conn);
#endif
            Database_write(conn);
            break;

        case 'g':
            if (argc != 4)
                die("Need an id to get", conn);
#ifdef DYNAMIC_VER
            if (id >= conn->db->max_rows) die("There's not that many records.", conn);
#endif
            Database_get(conn, id);
            break;

        case 's':
            if (argc != 6)
                die("Need id, name, email to set", conn);

#ifdef DYNAMIC_VER
            if (id >= conn->db->max_rows) die("There's not that many records.", conn);
#endif
            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn);
            break;

        case 'd':
            if (argc != 4)
                die("Need id to delete", conn);

#ifdef DYNAMIC_VER
            if (id >= conn->db->max_rows) die("There's not that many records.", conn);
#endif
            Database_delete(conn, id);
            Database_write(conn);
            break;

        case 'l':
            Database_list(conn);
            break;

        case 'f':
            if (argc != 4)
                die("Need name to find", conn);

            Database_find(conn, argv[3]);
            break;
        default:
            die("Invalid action: c=crete, g=get, s=set, d=del, l=list", conn);
    }

    Database_close(conn);

    return 0;
}
