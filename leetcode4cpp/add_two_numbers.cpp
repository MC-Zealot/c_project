#include "include/common_struct.h"
#include <iostream>

using namespace std;
/**
 *
 *
 *You are given two non-empty linked lists representing two non-negative integers. 
 The digits are stored in reverse order and each of their nodes contain a single digit. Add the two numbers and return it as a linked list.

 You may assume the two numbers do not contain any leading zero, except the number 0 itself.

Input: (2 -> 4 -> 3) + (5 -> 6 -> 4)
Output: 7 -> 0 -> 8
 *
 *
 *
 *
 *
 *
 */
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
	if(l1 == NULL){
		return l2;
	}else if(l2 == NULL){
		return l1;
	}
	ListNode* dummy = new ListNode(-1);
	ListNode* node = dummy;
	int carry = 0;
	while(l1 != NULL && l2 != NULL){
		int sum = l1->val + l2->val + carry;
		carry = sum / 10;
		int digit = sum % 10;
		ListNode* l = new ListNode(digit);
		node->next = l;
		node = node->next;
		l1 = l1->next;
		l2 = l2->next;
	}
	while(l1 != NULL){
		int sum = l1->val + carry;
		carry = sum / 10; 
		int digit = sum % 10; 
		ListNode* l = new ListNode(digit);
		node->next = l;
		node = node->next;
		l1 = l1->next;
	}
	while(l2 != NULL){
		int sum = l2->val + carry;
		carry = sum / 10;
		int digit = sum % 10;
		ListNode* l = new ListNode(digit);
		node->next = l;
		node = node->next;
		l2 = l2->next;
	}
	if(carry > 0){
		ListNode* l = new ListNode(carry);
		node->next = l;
		node = node->next;
	}
	return dummy->next;
}
int main(){
	cout<< "hello addTwoNumbers!"<<endl;
	ListNode* l1=new ListNode(3);
	cout<< l1->val<< endl;
	ListNode* l2 = new ListNode(4);

	cout << l2->val << endl;
	cout<< "begin"<< endl;
	ListNode* res = addTwoNumbers(l1, l2);

	while(res != NULL){
		cout<< res->val << endl;
		res = res->next;
	}

	cout<< "end"<< endl;
}
