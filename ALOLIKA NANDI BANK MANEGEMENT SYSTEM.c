#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ACCOUNT_FILE "nandi_bank_accounts.dat"
#define TRANSACTION_FILE "nandi_bank_transactions.dat"

struct Account {
    char name[50];
    char gender[15];
    char mobile[15];
    char pin[10];

    long long accountNumber;

    double balance;
    double savings;

    double paymentLimit;
    double todayPayment;
    char lastPaymentDate[11];
};

struct Transaction {
    long long accountNumber;
    char type[30];
    double amount;
    double balanceAfter;
    char date[11];
};

/* ---------- Function Prototypes ---------- */

void clearInputBuffer(void);
void pauseScreen(void);
void currentDate(char date[]);
int validPIN(const char pin[]);
int validMobile(const char mobile[]);

int accountExists(long long accountNumber);
long long generateAccountNumber(void);
int findAccount(long long accountNumber, struct Account *account);
int updateAccountInFile(struct Account account);

void saveTransaction(long long accountNumber, const char type[],
                     double amount, double balanceAfter);

void createAccount(void);
int login(struct Account *account);
void forgotPIN(void);

void accountInformation(struct Account account);
void balanceEnquiry(struct Account account);

void depositMoney(struct Account *account);
void withdrawMoney(struct Account *account);

void resetDailyPayment(struct Account *account);
void setPaymentLimit(struct Account *account);
void makePayment(struct Account *account);

void transferMoney(struct Account *account);
void saveMoney(struct Account *account);

void transactionHistory(struct Account account);

void updateAccount(struct Account *account);
void changePIN(struct Account *account);


/* ---------- Utility Functions ---------- */

void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

void pauseScreen(void)
{
    int ch;

    /* Remove any leftover input/newline first. */
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;

    printf("\nPress Enter to continue...");
    getchar();
}

void currentDate(char date[])
{
    time_t now = time(NULL);
    struct tm *info = localtime(&now);

    if (info != NULL)
        strftime(date, 11, "%Y-%m-%d", info);
    else
        strcpy(date, "0000-00-00");
}

int validPIN(const char pin[])
{
    int i;

    if (strlen(pin) != 4)
        return 0;

    for (i = 0; i < 4; i++)
    {
        if (pin[i] < '0' || pin[i] > '9')
            return 0;
    }

    return 1;
}

int validMobile(const char mobile[])
{
    int i;
    int len = (int)strlen(mobile);

    if (len < 10 || len > 14)
        return 0;

    for (i = 0; i < len; i++)
    {
        if (mobile[i] < '0' || mobile[i] > '9')
            return 0;
    }

    return 1;
}

/* ---------- Account File Functions ---------- */

int accountExists(long long accountNumber)
{
    FILE *file;
    struct Account account;

    file = fopen(ACCOUNT_FILE, "rb");

    if (file == NULL)
        return 0;

    while (fread(&account, sizeof(account), 1, file) == 1)
    {
        if (account.accountNumber == accountNumber)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

long long generateAccountNumber(void)
{
    long long number;

    do
    {
        number = 10000000LL + rand() % 90000000;
    }
    while (accountExists(number));

    return number;
}

int findAccount(long long accountNumber, struct Account *account)
{
    FILE *file;

    file = fopen(ACCOUNT_FILE, "rb");

    if (file == NULL)
        return 0;

    while (fread(account, sizeof(*account), 1, file) == 1)
    {
        if (account->accountNumber == accountNumber)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int updateAccountInFile(struct Account account)
{
    FILE *file;
    struct Account oldAccount;

    file = fopen(ACCOUNT_FILE, "rb+");

    if (file == NULL)
        return 0;

    while (fread(&oldAccount, sizeof(oldAccount), 1, file) == 1)
    {
        if (oldAccount.accountNumber == account.accountNumber)
        {
            fseek(file, -(long)sizeof(oldAccount), SEEK_CUR);

            if (fwrite(&account, sizeof(account), 1, file) == 1)
            {
                fclose(file);
                return 1;
            }

            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 0;
}

/* ---------- Transaction File ---------- */

void saveTransaction(long long accountNumber, const char type[],
                     double amount, double balanceAfter)
{
    FILE *file;
    struct Transaction transaction;

    file = fopen(TRANSACTION_FILE, "ab");

    if (file == NULL)
        return;

    transaction.accountNumber = accountNumber;

    strncpy(transaction.type, type, sizeof(transaction.type) - 1);
    transaction.type[sizeof(transaction.type) - 1] = '\0';

    transaction.amount = amount;
    transaction.balanceAfter = balanceAfter;

    currentDate(transaction.date);

    fwrite(&transaction, sizeof(transaction), 1, file);
    fclose(file);
}

/* ---------- Create Account ---------- */

void createAccount(void)
{
    struct Account account = {0};
    FILE *file;
    int genderChoice;

    printf("\n");
    printf("============================================\n");
    printf("             CREATE NEW ACCOUNT\n");
    printf("============================================\n");

    printf("Full Name: ");
    scanf(" %49[^\n]", account.name);

    do
    {
        printf("\nGender\n");
        printf("1. Male\n");
        printf("2. Female\n");
        printf("3. Other\n");
        printf("Choose: ");

        if (scanf("%d", &genderChoice) != 1)
        {
            clearInputBuffer();
            genderChoice = 0;
        }

        if (genderChoice == 1)
            strcpy(account.gender, "Male");
        else if (genderChoice == 2)
            strcpy(account.gender, "Female");
        else if (genderChoice == 3)
            strcpy(account.gender, "Other");
        else
            printf("Invalid gender choice.\n");

    } while (genderChoice < 1 || genderChoice > 3);

    do
    {
        printf("Mobile Number: ");
        scanf("%14s", account.mobile);

        if (!validMobile(account.mobile))
            printf("Enter a valid mobile number.\n");

    } while (!validMobile(account.mobile));

    do
    {
        printf("Set 4-Digit PIN: ");
        scanf("%9s", account.pin);

        if (!validPIN(account.pin))
            printf("PIN must contain exactly 4 digits.\n");

    } while (!validPIN(account.pin));

    do
    {
        printf("Initial Balance: Rs. ");

        if (scanf("%lf", &account.balance) != 1)
        {
            clearInputBuffer();
            account.balance = -1;
        }

        if (account.balance < 0)
            printf("Balance cannot be negative.\n");

    } while (account.balance < 0);

    account.accountNumber = generateAccountNumber();
    account.savings = 0.0;
    account.paymentLimit = 0.0;
    account.todayPayment = 0.0;

    currentDate(account.lastPaymentDate);

    file = fopen(ACCOUNT_FILE, "ab");

    if (file == NULL)
    {
        printf("\nUnable to save account.\n");
        pauseScreen();
        return;
    }

    fwrite(&account, sizeof(account), 1, file);
    fclose(file);

    if (account.balance > 0)
    {
        saveTransaction(account.accountNumber,
                        "Initial Deposit",
                        account.balance,
                        account.balance);
    }

    printf("\n============================================\n");
    printf("          ACCOUNT CREATED SUCCESSFULLY\n");
    printf("============================================\n");
    printf("Account Number : %lld\n", account.accountNumber);
    printf("Initial Balance: Rs. %.2lf\n", account.balance);
    printf("\nIMPORTANT: Remember your Account Number.\n");

    pauseScreen();
}

/* ---------- Login ---------- */

int login(struct Account *account)
{
    long long accountNumber;
    char pin[10];

    printf("\n");
    printf("============================================\n");
    printf("                    LOGIN\n");
    printf("============================================\n");

    printf("Account Number: ");
    if (scanf("%lld", &accountNumber) != 1)
    {
        clearInputBuffer();
        printf("Invalid account number.\n");
        return 0;
    }

    printf("PIN: ");
    scanf("%9s", pin);

    if (!findAccount(accountNumber, account))
    {
        printf("\nAccount not found.\n");
        return 0;
    }

    if (strcmp(account->pin, pin) != 0)
    {
        printf("\nInvalid PIN.\n");
        return 0;
    }

    resetDailyPayment(account);

    printf("\nLogin successful. Welcome, %s!\n", account->name);

    return 1;
}

/* ---------- Forgot PIN ---------- */

void forgotPIN(void)
{
    long long accountNumber;
    char mobile[15];
    char newPIN[10];
    char confirmPIN[10];

    struct Account account;

    printf("\n");
    printf("============================================\n");
    printf("                  FORGOT PIN\n");
    printf("============================================\n");

    printf("Account Number: ");

    if (scanf("%lld", &accountNumber) != 1)
    {
        clearInputBuffer();
        printf("Invalid account number.\n");
        return;
    }

    if (!findAccount(accountNumber, &account))
    {
        printf("Account not found.\n");
        return;
    }

    printf("Registered Mobile Number: ");
    scanf("%14s", mobile);

    if (strcmp(account.mobile, mobile) != 0)
    {
        printf("Mobile number does not match.\n");
        return;
    }

    do
    {
        printf("New 4-Digit PIN: ");
        scanf("%9s", newPIN);

        if (!validPIN(newPIN))
            printf("PIN must contain exactly 4 digits.\n");

    } while (!validPIN(newPIN));

    printf("Confirm New PIN: ");
    scanf("%9s", confirmPIN);

    if (strcmp(newPIN, confirmPIN) != 0)
    {
        printf("PIN confirmation failed.\n");
        return;
    }

    strcpy(account.pin, newPIN);

    if (updateAccountInFile(account))
        printf("\nPIN reset successful.\n");
    else
        printf("\nUnable to update PIN.\n");

    pauseScreen();
}

/* ---------- Account Information ---------- */

void accountInformation(struct Account account)
{
    printf("\n");
    printf("============================================\n");
    printf("             ACCOUNT INFORMATION\n");
    printf("============================================\n");

    printf("Name           : %s\n", account.name);
    printf("Gender         : %s\n", account.gender);
    printf("Mobile         : %s\n", account.mobile);
    printf("Account Number : %lld\n", account.accountNumber);
    printf("Main Balance   : Rs. %.2lf\n", account.balance);
    printf("Savings        : Rs. %.2lf\n", account.savings);

    if (account.paymentLimit == 0)
        printf("Daily Pay Limit: No Limit\n");
    else
        printf("Daily Pay Limit: Rs. %.2lf\n", account.paymentLimit);

    /* PIN is intentionally never displayed. */

    pauseScreen();
}

/* ---------- Balance Enquiry ---------- */

void balanceEnquiry(struct Account account)
{
    printf("\n");
    printf("============================================\n");
    printf("               BALANCE ENQUIRY\n");
    printf("============================================\n");

    printf("Main Balance : Rs. %.2lf\n", account.balance);
    printf("Savings      : Rs. %.2lf\n", account.savings);
    printf("--------------------------------------------\n");
    printf("Total Money  : Rs. %.2lf\n",
           account.balance + account.savings);

    pauseScreen();
}

/* ---------- Deposit ---------- */

void depositMoney(struct Account *account)
{
    double amount;

    printf("\n");
    printf("========== DEPOSIT MONEY ==========\n");

    printf("Deposit Amount: Rs. ");

    if (scanf("%lf", &amount) != 1)
    {
        clearInputBuffer();
        printf("Invalid amount.\n");
        return;
    }

    if (amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
        return;
    }

    account->balance += amount;

    if (updateAccountInFile(*account))
    {
        saveTransaction(account->accountNumber,
                        "Deposit",
                        amount,
                        account->balance);

        printf("Deposit successful!\n");
        printf("New Balance: Rs. %.2lf\n", account->balance);
    }
    else
    {
        printf("Deposit could not be saved.\n");
    }

    pauseScreen();
}

/* ---------- Withdraw ---------- */

void withdrawMoney(struct Account *account)
{
    double amount;

    printf("\n");
    printf("========== WITHDRAW MONEY ==========\n");

    printf("Withdraw Amount: Rs. ");

    if (scanf("%lf", &amount) != 1)
    {
        clearInputBuffer();
        printf("Invalid amount.\n");
        return;
    }

    if (amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
        return;
    }

    if (amount > account->balance)
    {
        printf("Insufficient main balance.\n");
        return;
    }

    account->balance -= amount;

    if (updateAccountInFile(*account))
    {
        saveTransaction(account->accountNumber,
                        "Withdrawal",
                        amount,
                        account->balance);

        printf("Withdrawal successful!\n");
        printf("Remaining Balance: Rs. %.2lf\n", account->balance);
    }
    else
    {
        printf("Withdrawal could not be saved.\n");
    }

    pauseScreen();
}

/* ---------- Daily Payment Reset ---------- */

void resetDailyPayment(struct Account *account)
{
    char today[11];

    currentDate(today);

    if (strcmp(account->lastPaymentDate, today) != 0)
    {
        account->todayPayment = 0.0;
        strcpy(account->lastPaymentDate, today);

        updateAccountInFile(*account);
    }
}

/* ---------- Payment Limit ---------- */

void setPaymentLimit(struct Account *account)
{
    double limit;

    resetDailyPayment(account);

    printf("\n");
    printf("========== PAYMENT LIMIT ==========\n");

    if (account->paymentLimit == 0)
        printf("Current Limit: No Limit\n");
    else
        printf("Current Limit: Rs. %.2lf\n", account->paymentLimit);

    printf("Used Today: Rs. %.2lf\n", account->todayPayment);

    printf("\nNew Daily Payment Limit: Rs. ");

    if (scanf("%lf", &limit) != 1)
    {
        clearInputBuffer();
        printf("Invalid limit.\n");
        return;
    }

    if (limit < 0)
    {
        printf("Limit cannot be negative.\n");
        return;
    }

    account->paymentLimit = limit;

    /* A new limit starts a fresh spending cycle. */
    account->todayPayment = 0.0;

    if (updateAccountInFile(*account))
    {
        if (limit == 0)
            printf("Payment limit removed. Payments are now unrestricted.\n");
        else
            printf("New daily payment limit: Rs. %.2lf\n", limit);
    }
    else
    {
        printf("Could not update payment limit.\n");
    }

    pauseScreen();
}

/* ---------- Make Payment ---------- */

void makePayment(struct Account *account)
{
    double amount;
    double remaining;
    char choice;

    resetDailyPayment(account);

    printf("\n");
    printf("========== MAKE PAYMENT ==========\n");

    printf("Main Balance : Rs. %.2lf\n", account->balance);

    if (account->paymentLimit == 0)
    {
        printf("Daily Limit  : No Limit\n");
    }
    else
    {
        remaining = account->paymentLimit - account->todayPayment;

        if (remaining < 0)
            remaining = 0;

        printf("Daily Limit  : Rs. %.2lf\n", account->paymentLimit);
        printf("Used Today   : Rs. %.2lf\n", account->todayPayment);
        printf("Remaining    : Rs. %.2lf\n", remaining);
    }

    printf("\nPayment Amount: Rs. ");

    if (scanf("%lf", &amount) != 1)
    {
        clearInputBuffer();
        printf("Invalid amount.\n");
        return;
    }

    if (amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
        return;
    }

    if (amount > account->balance)
    {
        printf("Insufficient balance.\n");
        return;
    }

    if (account->paymentLimit > 0)
    {
        remaining = account->paymentLimit - account->todayPayment;

        if (remaining < 0)
            remaining = 0;

        if (amount > remaining)
        {
            printf("\nWARNING: Payment limit will be crossed by Rs. %.2lf.\n",
                   amount - remaining);

            printf("Do you want to continue? (Y/N): ");
            scanf(" %c", &choice);

            if (choice != 'Y' && choice != 'y')
            {
                printf("Payment cancelled.\n");
                pauseScreen();
                return;
            }
        }
    }

    account->balance -= amount;
    account->todayPayment += amount;

    if (updateAccountInFile(*account))
    {
        saveTransaction(account->accountNumber,
                        "Payment",
                        amount,
                        account->balance);

        printf("\nPayment successful!\n");
        printf("Paid Amount   : Rs. %.2lf\n", amount);
        printf("Balance       : Rs. %.2lf\n", account->balance);

        if (account->paymentLimit > 0)
        {
            remaining = account->paymentLimit - account->todayPayment;

            if (remaining < 0)
                remaining = 0;

            printf("Limit Remaining: Rs. %.2lf\n", remaining);
        }
    }
    else
    {
        printf("Payment could not be saved.\n");
    }

    pauseScreen();
}

/* ---------- Money Transfer ---------- */

void transferMoney(struct Account *account)
{
    long long receiverNumber;
    double amount;
    struct Account receiver;

    printf("\n");
    printf("========== MONEY TRANSFER ==========\n");

    printf("Receiver Account Number: ");

    if (scanf("%lld", &receiverNumber) != 1)
    {
        clearInputBuffer();
        printf("Invalid account number.\n");
        return;
    }

    if (receiverNumber == account->accountNumber)
    {
        printf("You cannot transfer money to your own account.\n");
        return;
    }

    if (!findAccount(receiverNumber, &receiver))
    {
        printf("Receiver account not found.\n");
        return;
    }

    printf("Transfer Amount: Rs. ");

    if (scanf("%lf", &amount) != 1)
    {
        clearInputBuffer();
        printf("Invalid amount.\n");
        return;
    }

    if (amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
        return;
    }

    if (amount > account->balance)
    {
        printf("Insufficient balance.\n");
        return;
    }

    account->balance -= amount;
    receiver.balance += amount;

    /*
       Both account records are updated.
       For a student project this demonstrates file handling
       and two-account transactions.
    */
    if (!updateAccountInFile(*account))
    {
        account->balance += amount;
        printf("Transfer failed. Sender account was not updated.\n");
        return;
    }

    if (!updateAccountInFile(receiver))
    {
        /*
           Try to restore sender if receiver update fails.
        */
        account->balance += amount;
        updateAccountInFile(*account);

        printf("Transfer failed. Please try again.\n");
        return;
    }

    saveTransaction(account->accountNumber,
                    "Transfer Sent",
                    amount,
                    account->balance);

    saveTransaction(receiver.accountNumber,
                    "Transfer Received",
                    amount,
                    receiver.balance);

    printf("\nTransfer successful!\n");
    printf("Amount Sent : Rs. %.2lf\n", amount);
    printf("New Balance : Rs. %.2lf\n", account->balance);

    pauseScreen();
}

/* ---------- Save Money ---------- */

void saveMoney(struct Account *account)
{
    double amount;

    printf("\n");
    printf("========== SAVE MONEY ==========\n");

    printf("Main Balance : Rs. %.2lf\n", account->balance);
    printf("Savings      : Rs. %.2lf\n", account->savings);

    printf("\nAmount to Save: Rs. ");

    if (scanf("%lf", &amount) != 1)
    {
        clearInputBuffer();
        printf("Invalid amount.\n");
        return;
    }

    if (amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
        return;
    }

    if (amount > account->balance)
    {
        printf("Insufficient main balance.\n");
        return;
    }

    account->balance -= amount;
    account->savings += amount;

    if (updateAccountInFile(*account))
    {
        saveTransaction(account->accountNumber,
                        "Savings",
                        amount,
                        account->balance);

        printf("\nMoney saved successfully!\n");
        printf("Main Balance : Rs. %.2lf\n", account->balance);
        printf("Savings      : Rs. %.2lf\n", account->savings);
    }
    else
    {
        printf("Savings transaction could not be saved.\n");
    }

    pauseScreen();
}

/* ---------- Transaction History ---------- */

void transactionHistory(struct Account account)
{
    FILE *file;
    struct Transaction transaction;
    int found = 0;

    printf("\n");
    printf("====================================================================\n");
    printf("                     TRANSACTION HISTORY\n");
    printf("====================================================================\n");

    file = fopen(TRANSACTION_FILE, "rb");

    if (file == NULL)
    {
        printf("No transactions found.\n");
        pauseScreen();
        return;
    }

    printf("%-12s %-22s %-15s %-15s\n",
           "DATE", "TYPE", "AMOUNT", "BALANCE");
    printf("--------------------------------------------------------------------\n");

    while (fread(&transaction, sizeof(transaction), 1, file) == 1)
    {
        if (transaction.accountNumber == account.accountNumber)
        {
            found = 1;

            printf("%-12s %-22s Rs.%-11.2lf Rs.%-11.2lf\n",
                   transaction.date,
                   transaction.type,
                   transaction.amount,
                   transaction.balanceAfter);
        }
    }

    fclose(file);

    if (!found)
        printf("No transactions found for this account.\n");

    pauseScreen();
}

/* ---------- Update Account ---------- */

void updateAccount(struct Account *account)
{
    int choice;
    int genderChoice;
    char newName[50];
    char newMobile[15];

    printf("\n");
    printf("========== UPDATE ACCOUNT ==========\n");
    printf("1. Update Name\n");
    printf("2. Update Gender\n");
    printf("3. Update Mobile Number\n");
    printf("4. Back\n");
    printf("Choose: ");

    if (scanf("%d", &choice) != 1)
    {
        clearInputBuffer();
        printf("Invalid choice.\n");
        return;
    }

    switch (choice)
    {
        case 1:
            printf("New Name: ");
            scanf(" %49[^\n]", newName);

            strcpy(account->name, newName);

            if (updateAccountInFile(*account))
                printf("Name updated successfully.\n");
            else
                printf("Could not update name.\n");
            break;

        case 2:
            printf("\n1. Male\n");
            printf("2. Female\n");
            printf("3. Other\n");
            printf("Choose: ");

            if (scanf("%d", &genderChoice) != 1)
            {
                clearInputBuffer();
                printf("Invalid choice.\n");
                return;
            }

            if (genderChoice == 1)
                strcpy(account->gender, "Male");
            else if (genderChoice == 2)
                strcpy(account->gender, "Female");
            else if (genderChoice == 3)
                strcpy(account->gender, "Other");
            else
            {
                printf("Invalid gender choice.\n");
                return;
            }

            if (updateAccountInFile(*account))
                printf("Gender updated successfully.\n");
            else
                printf("Could not update gender.\n");
            break;

        case 3:
            do
            {
                printf("New Mobile Number: ");
                scanf("%14s", newMobile);

                if (!validMobile(newMobile))
                    printf("Enter a valid mobile number.\n");

            } while (!validMobile(newMobile));

            strcpy(account->mobile, newMobile);

            if (updateAccountInFile(*account))
                printf("Mobile number updated successfully.\n");
            else
                printf("Could not update mobile number.\n");
            break;

        case 4:
            return;

        default:
            printf("Invalid choice.\n");
    }

    pauseScreen();
}

/* ---------- Change PIN ---------- */

void changePIN(struct Account *account)
{
    char oldPIN[10];
    char newPIN[10];
    char confirmPIN[10];

    printf("\n");
    printf("========== CHANGE PIN ==========\n");

    printf("Current PIN: ");
    scanf("%9s", oldPIN);

    if (strcmp(oldPIN, account->pin) != 0)
    {
        printf("Incorrect current PIN.\n");
        pauseScreen();
        return;
    }

    do
    {
        printf("New 4-Digit PIN: ");
        scanf("%9s", newPIN);

        if (!validPIN(newPIN))
            printf("PIN must contain exactly 4 digits.\n");

    } while (!validPIN(newPIN));

    printf("Confirm New PIN: ");
    scanf("%9s", confirmPIN);

    if (strcmp(newPIN, confirmPIN) != 0)
    {
        printf("PIN confirmation failed.\n");
        pauseScreen();
        return;
    }

    strcpy(account->pin, newPIN);

    if (updateAccountInFile(*account))
        printf("PIN changed successfully.\n");
    else
        printf("Could not change PIN.\n");

    pauseScreen();
}

/* ---------- Main Menu ---------- */

int main(void)
{
    int choice;
    int loggedIn = 0;
    struct Account loggedInAccount;

    srand((unsigned)time(NULL));

    while (1)
    {
        if (loggedIn)
            resetDailyPayment(&loggedInAccount);

        printf("\n\n");
        printf("============================================================\n");
        printf("                         NANDI BANK\n");
        printf("                BANK ACCOUNT MANAGEMENT SYSTEM\n");
        printf("============================================================\n");

        if (loggedIn)
        {
            printf("Logged in as : %s\n", loggedInAccount.name);
            printf("Account No.  : %lld\n", loggedInAccount.accountNumber);
        }
        else
        {
            printf("Status       : Not logged in\n");
        }

        printf("------------------------------------------------------------\n");
        printf("1.  Create New Account\n");
        printf("2.  Login\n");
        printf("3.  Forgot PIN\n");
        printf("4.  Account Information\n");
        printf("5.  Balance Enquiry\n");
        printf("6.  Update Account\n");
        printf("7.  Deposit Money\n");
        printf("8.  Withdraw Money\n");
        printf("9.  Make Payment\n");
        printf("10. Set Payment Limit\n");
        printf("11. Money Transfer\n");
        printf("12. Save Money\n");
        printf("13. Transaction History\n");
        printf("14. Change PIN\n");
        printf("15. Logout\n");
        printf("16. Exit\n");
        printf("============================================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            clearInputBuffer();
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice)
        {
            case 1:
                createAccount();
                break;

            case 2:
                if (loggedIn)
                {
                    printf("\nYou are already logged in. Please logout first.\n");
                    pauseScreen();
                }
                else if (login(&loggedInAccount))
                {
                    loggedIn = 1;
                }
                break;

            case 3:
                forgotPIN();
                break;

            case 4:
                if (loggedIn) accountInformation(loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 5:
                if (loggedIn) balanceEnquiry(loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 6:
                if (loggedIn) updateAccount(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 7:
                if (loggedIn) depositMoney(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 8:
                if (loggedIn) withdrawMoney(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 9:
                if (loggedIn) makePayment(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 10:
                if (loggedIn) setPaymentLimit(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 11:
                if (loggedIn) transferMoney(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 12:
                if (loggedIn) saveMoney(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 13:
                if (loggedIn) transactionHistory(loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 14:
                if (loggedIn) changePIN(&loggedInAccount);
                else { printf("\nPlease login first.\n"); pauseScreen(); }
                break;

            case 15:
                if (loggedIn)
                {
                    loggedIn = 0;
                    printf("\nLogged out successfully.\n");
                    pauseScreen();
                }
                else
                {
                    printf("\nNo account is currently logged in.\n");
                    pauseScreen();
                }
                break;

            case 16:
                printf("\nThank you for using Nandi Bank!\n");
                printf("Have a great day.\n");
                return 0;

            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}
