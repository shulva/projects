//
// Created by shulva on 24-9-5.
//


struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:

    ListNode* head = new ListNode();

    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {

        int Node_val1=0;
        int Node_val2=0;

        ListNode* next_ptr = new ListNode();
        head = next_ptr;

        int carry =0;
        int sum =0;

        do{

            if(l1!= nullptr)
                Node_val1 = l1->val;
            else
                Node_val1 = 0;

            if(l2!= nullptr)
                Node_val2 = l2->val;
            else
                Node_val2 = 0;


            if (Node_val1 + Node_val2+carry >= 10) {
                sum = Node_val2 + Node_val1+ carry - 10 ;
                carry = 1;
            }
            else{
                sum = Node_val2+Node_val1+carry;
                carry = 0;
            }


            ListNode *loop_next = new ListNode();
            next_ptr->val = sum;

            if (l1!= nullptr)
                l1 = l1->next;

            if (l2!= nullptr)
                l2 = l2->next;

            if((l1 != nullptr)||(l2 != nullptr)||(carry != 0)){
                next_ptr->next = loop_next;
                next_ptr = loop_next;
                next_ptr->val = carry;
            }

        }while((l1!= nullptr) || (l2!= nullptr));

        return head;
    }
};