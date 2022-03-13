

// TODO להזיז לקובץ השני תוך טיפול בשגיאה
typedef enum boolean {
    false = 0, true
} boolean;

typedef struct Attribute {
    boolean code;
    boolean data;
    boolean entry;
    boolean external;
} Attribute;

struct data {
    int value;
    int baseAddress;
    int offset;
    Attribute attributes;
};

struct Node {
    char *symbol;
    struct data data;
    struct Node *next;
};

