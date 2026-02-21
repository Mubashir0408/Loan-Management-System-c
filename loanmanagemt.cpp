#include <iostream>
#include <string>
using namespace std;

struct Transaction {
    int id;
    int amount;
    int personId;  
    int dayBorrowed;
    int daysAllowed;
    float finePercent;
    int dayReturned;
    bool isReturned;
    bool isLoan;  
    Transaction* next;
    
    Transaction(int tid, int amt, int pid, int d1, int d2, float f, bool loan) {
        id = tid;
        amount = amt;
        personId = pid;
        dayBorrowed = d1;
        daysAllowed = d2;
        finePercent = f;
        dayReturned = -1;
        isReturned = false;
        isLoan = loan;
        next = NULL;
    }
};

struct User {
    int id;
    string name;
    bool active;
    Transaction* loansHead;
    Transaction* paymentsHead;
    User* left;
    User* right;
    
    User(int uid, string uname) { 
        id = uid;
        name = uname;
        active = true;
        loansHead = NULL;
        paymentsHead = NULL;
        left = NULL;
        right = NULL;
    }
};

class LoanManagementSystem {
private:
    User* root;
    int transactionCounter;
    
    User* insertUser(User* node, int id, string name) {
        if (node == NULL) {
            return new User(id, name);
        }
        if (id < node->id) {
            node->left = insertUser(node->left, id, name);
        } else if (id > node->id) {
            node->right = insertUser(node->right, id, name);
        }
        return node;
    }
    
    User* searchUser(User* node, int id) {
        if (node == NULL || node->id == id) {
            return node;
        }
        if (id < node->id) {
            return searchUser(node->left, id);
        }
        return searchUser(node->right, id);
    }
    
    void inorderTraversal(User* node) {
        if (node != NULL) {
            inorderTraversal(node->left);
            if (node->active) {
                cout << "ID: " << node->id << ", Name: " << node->name << endl;
            }
            inorderTraversal(node->right);
        }
    }
    
    void addTransaction(Transaction*& head, int id, int amt, int pid, int d1, int d2, float f, bool loan) {
        Transaction* newTrans = new Transaction(id, amt, pid, d1, d2, f, loan);
        newTrans->next = head;
        head = newTrans;
    }
    
    Transaction* findTransaction(Transaction* head, int tid) {
        Transaction* temp = head;
        while (temp != NULL) {
            if (temp->id == tid) {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }
    
    void displayTransactions(Transaction* head, string type) {
        Transaction* temp = head;
        if (temp == NULL) {
            cout << "  No " << type << " found." << endl;
            return;
        }
        while (temp != NULL) {
            cout << "  TID: " << temp->id << " | Amount: " << temp->amount 
                 << " | Person: " << temp->personId << " | Day: " << temp->dayBorrowed;
            if (temp->isReturned) {
                cout << " | RETURNED on day " << temp->dayReturned;
            } else {
                cout << " | PENDING";
            }
            cout << endl;
            temp = temp->next;
        }
    }
    
public:
    LoanManagementSystem() {
        root = NULL;
        transactionCounter = 1;
    }
    
    void addUser(int id, string name) {
        User* existing = searchUser(root, id);
        if (existing != NULL) {
            if (!existing->active) {
                existing->active = true;
                existing->name = name;
                cout << "User reactivated successfully!" << endl;
            } else {
                cout << "User with this ID already exists!" << endl;
            }
            return;
        }
        root = insertUser(root, id, name);
        cout << "User added successfully!" << endl;
    }
    
    void updateUser(int id, string newName) {
        User* user = searchUser(root, id);
        if (user == NULL || !user->active) {
            cout << "User not found!" << endl;
            return;
        }
        user->name = newName;
        cout << "User updated successfully!" << endl;
    }
    
    void deleteUser(int id) {
        User* user = searchUser(root, id);
        if (user == NULL || !user->active) {
            cout << "User not found!" << endl;
            return;
        }
        user->active = false;
        cout << "User deleted successfully!" << endl;
    }
    
    void searchUserById(int id) {
        User* user = searchUser(root, id);
        if (user == NULL || !user->active) {
            cout << "User not found!" << endl;
            return;
        }
        cout << "\nUser Details:" << endl;
        cout << "ID: " << user->id << ", Name: " << user->name << endl;
    }
    
    void listUsers() {
        cout << "\nActive Users:" << endl;
        inorderTraversal(root);
    }
    
    void borrowMoney(int p1, int p2, int amt, int day, int daysAllowed, float finePercent) {
        User* borrower = searchUser(root, p1);
        User* lender = searchUser(root, p2);
        
        if (borrower == NULL || !borrower->active || lender == NULL || !lender->active) {
            cout << "One or both users not found!" << endl;
            return;
        }
        
        int tid = transactionCounter++;
        
        addTransaction(borrower->loansHead, tid, amt, p2, day, daysAllowed, finePercent, true);
        
        addTransaction(lender->paymentsHead, tid, amt, p1, day, daysAllowed, finePercent, false);
        
        cout << "Loan created successfully! Transaction ID: " << tid << endl;
    }
    
    void returnLoan(int tid, int p1, int p2, int dayReturned) {
        User* payer = searchUser(root, p1);
        User* receiver = searchUser(root, p2);
        
        if (payer == NULL || !payer->active || receiver == NULL || !receiver->active) {
            cout << "One or both users not found!" << endl;
            return;
        }
        
        Transaction* loan = findTransaction(payer->loansHead, tid);
        Transaction* payment = findTransaction(receiver->paymentsHead, tid);
        
        if (loan == NULL || payment == NULL) {
            cout << "Transaction not found!" << endl;
            return;
        }
        
        if (loan->isReturned) {
            cout << "Loan already returned!" << endl;
            return;
        }
        
        loan->isReturned = true;
        loan->dayReturned = dayReturned;
        payment->isReturned = true;
        payment->dayReturned = dayReturned;
        
        int fine = 0;
        if (dayReturned > loan->dayBorrowed + loan->daysAllowed) {
            fine = (loan->amount * loan->finePercent) / 100;
        }
        
        cout << "Loan returned successfully!" << endl;
        cout << "Amount: " << loan->amount;
        if (fine > 0) {
            cout << " + Fine: " << fine << " = Total: " << (loan->amount + fine);
        }
        cout << endl;
    }
    
    void transferLoan(int tid, int p1, int p2) {
        User* newDebtor = searchUser(root, p1);
        User* oldDebtor = searchUser(root, p2);
        
        if (newDebtor == NULL || !newDebtor->active || oldDebtor == NULL || !oldDebtor->active) {
            cout << "One or both users not found!" << endl;
            return;
        }
        
        Transaction* loan = findTransaction(oldDebtor->loansHead, tid);
        if (loan == NULL || loan->isReturned) {
            cout << "Transaction not found or already returned!" << endl;
            return;
        }
        
        User* lender = searchUser(root, loan->personId);
        Transaction* payment = findTransaction(lender->paymentsHead, tid);
        
        loan->personId = p1;
        payment->personId = p1;
        
        addTransaction(newDebtor->loansHead, tid, loan->amount, lender->id, 
                      loan->dayBorrowed, loan->daysAllowed, loan->finePercent, true);
        
        cout << "Loan transferred successfully from " << p2 << " to " << p1 << endl;
    }
    
    void checkAccount(int id) {
        User* user = searchUser(root, id);
        if (user == NULL || !user->active) {
            cout << "User not found!" << endl;
            return;
        }
        
        cout << "\n=== Account Summary for " << user->name << " (ID: " << id << ") ===" << endl;
        
        cout << "\nLoans (Money to Pay):" << endl;
        displayTransactions(user->loansHead, "loans");
        
        cout << "\nPayments (Money to Receive):" << endl;
        displayTransactions(user->paymentsHead, "payments");
    }
    
    void checkTransactionsBetween(int p1, int p2) {
        User* user1 = searchUser(root, p1);
        User* user2 = searchUser(root, p2);
        
        if (user1 == NULL || !user1->active || user2 == NULL || !user2->active) {
            cout << "One or both users not found!" << endl;
            return;
        }
        
        cout << "\n=== Transactions between " << user1->name << " and " << user2->name << " ===" << endl;
        
        Transaction* temp = user1->loansHead;
        bool found = false;
        while (temp != NULL) {
            if (temp->personId == p2) {
                cout << "TID: " << temp->id << " | " << user1->name << " owes " << user2->name 
                     << " | Amount: " << temp->amount;
                if (temp->isReturned) cout << " | RETURNED";
                else cout << " | PENDING";
                cout << endl;
                found = true;
            }
            temp = temp->next;
        }
        
        temp = user1->paymentsHead;
        while (temp != NULL) {
            if (temp->personId == p2) {
                cout << "TID: " << temp->id << " | " << user2->name << " owes " << user1->name 
                     << " | Amount: " << temp->amount;
                if (temp->isReturned) cout << " | RETURNED";
                else cout << " | PENDING";
                cout << endl;
                found = true;
            }
            temp = temp->next;
        }
        
        if (!found) {
            cout << "No transactions found between these users." << endl;
        }
    }
    
    void summarizeSystem(User* node) {
        if (node != NULL) {
            summarizeSystem(node->left);
            if (node->active) {
                cout << "\nUser: " << node->name << " (ID: " << node->id << ")" << endl;
                
                int totalOwes = 0, totalReceives = 0;
                Transaction* temp = node->loansHead;
                while (temp != NULL) {
                    if (!temp->isReturned) totalOwes += temp->amount;
                    temp = temp->next;
                }
                
                temp = node->paymentsHead;
                while (temp != NULL) {
                    if (!temp->isReturned) totalReceives += temp->amount;
                    temp = temp->next;
                }
                
                cout << "  Total Owes: " << totalOwes << endl;
                cout << "  Total to Receive: " << totalReceives << endl;
                cout << "  Net Balance: " << (totalReceives - totalOwes) << endl;
            }
            summarizeSystem(node->right);
        }
    }
    
    void summarizeSystemAccounts() {
        cout << "\n========== SYSTEM SUMMARY ==========" << endl;
        summarizeSystem(root);
    }
};

int main() {
    LoanManagementSystem lms;
    int choice, id, id2, amt, day, days;
    float fine;
    string name;
    
    while (true) {
        cout << "\n========== LOAN MANAGEMENT SYSTEM ==========" << endl;
        cout << "1.  Add User" << endl;
        cout << "2.  Update User" << endl;
        cout << "3.  Delete User" << endl;
        cout << "4.  Search User" << endl;
        cout << "5.  List All Users" << endl;
        cout << "6.  Borrow Money" << endl;
        cout << "7.  Return Loan" << endl;
        cout << "8.  Transfer Loan" << endl;
        cout << "9.  Check Account" << endl;
        cout << "10. Check Transactions Between Two Users" << endl;
        cout << "11. Summarize System" << endl;
        cout << "0.  Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Enter User ID: ";
                cin >> id;
                cout << "Enter User Name: ";
                cin.ignore();
                getline(cin, name);
                lms.addUser(id, name);
                break;
                
            case 2:
                cout << "Enter User ID: ";
                cin >> id;
                cout << "Enter New Name: ";
                cin.ignore();
                getline(cin, name);
                lms.updateUser(id, name);
                break;
                
            case 3:
                cout << "Enter User ID to Delete: ";
                cin >> id;
                lms.deleteUser(id);
                break;
                
            case 4:
                cout << "Enter User ID: ";
                cin >> id;
                lms.searchUserById(id);
                break;
                
            case 5:
                lms.listUsers();
                break;
                
            case 6:
                cout << "Enter Borrower ID: ";
                cin >> id;
                cout << "Enter Lender ID: ";
                cin >> id2;
                cout << "Enter Amount: ";
                cin >> amt;
                cout << "Enter Day Number (when borrowed): ";
                cin >> day;
                cout << "Enter Days Allowed (without fine): ";
                cin >> days;
                cout << "Enter Fine Percentage: ";
                cin >> fine;
                lms.borrowMoney(id, id2, amt, day, days, fine);
                break;
                
            case 7:
                cout << "Enter Transaction ID: ";
                cin >> id;
                cout << "Enter Payer ID: ";
                cin >> id2;
                int receiver;
                cout << "Enter Receiver ID: ";
                cin >> receiver;
                cout << "Enter Day Number (when returned): ";
                cin >> day;
                lms.returnLoan(id, id2, receiver, day);
                break;
                
            case 8:
                cout << "Enter Transaction ID: ";
                cin >> id;
                cout << "Enter New Debtor ID: ";
                cin >> id2;
                int oldDebtor;
                cout << "Enter Old Debtor ID: ";
                cin >> oldDebtor;
                lms.transferLoan(id, id2, oldDebtor);
                break;
                
            case 9:
                cout << "Enter User ID: ";
                cin >> id;
                lms.checkAccount(id);
                break;
                
            case 10:
                cout << "Enter First User ID: ";
                cin >> id;
                cout << "Enter Second User ID: ";
                cin >> id2;
                lms.checkTransactionsBetween(id, id2);
                break;
                
            case 11:
                lms.summarizeSystemAccounts();
                break;
                
            case 0:
                cout << "Exiting..." << endl;
                return 0;
                
            default:
                cout << "Invalid choice!" << endl;
        }
    }
    
    return 0;
}
