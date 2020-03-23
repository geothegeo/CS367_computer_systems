// George Tang, G01068335
// CS 367-002
// Project 3

#include <stdio.h>
#include <stdlib.h>
#include "memory_system.h"
/* Feel free to implement additional header files as needed */

typedef struct tlb{
  int index;
  int valid;
  int tag;
  int ppn;
} tlb;

typedef struct pt{
  int vpn;
  int valid;
  int ppn;
} pt;

typedef struct cache{
  int index;
  int valid1;
  int tag1;
  int data1;
  int valid2;
  int tag2;
  int data2;   
} cache;

tlb *arr_tlb;
pt *arr_pt;
cache *arr_cache;

void
initialize() {
/* if there is any initialization you would like to have, do it here */
/*  This is called for you in the main program */
  int a, b;
  
  arr_tlb = malloc(16 * sizeof(tlb));
  arr_pt = malloc(512 * sizeof(pt));
  arr_cache = malloc(32 * sizeof(cache));

  for(a = 0; a < 16; a++) {
    arr_tlb[a].index = a;
    arr_tlb[a].valid = 0;
    arr_tlb[a].tag = 0;
    arr_tlb[a].ppn = 0;
  }

  for(a = 0; a < 512; a++) {
    arr_pt[a].vpn = a;
    arr_pt[a].valid = 0;
    arr_pt[a].ppn = 0;
  }

  for(a = 0; a < 32; a++) {
    arr_cache[a].index = a;
    arr_cache[a].valid1 = 0;
    arr_cache[a].tag1 = 0;
    arr_cache[a].data1 = 0;
    arr_cache[a].valid2 = 0;
    arr_cache[a].tag2 = 0;
    arr_cache[a].data2 = 0; 
  }
}

/* You will implement the two functions below:
 *     * you may add as many additional functions as you like
 *     * you may add header files for your data structures
 *     * you MUST call the relevant log_entry() functions (described below)
 *          or you will not receive credit for all of your hard work!
 */

int
get_physical_address(int virt_address) {
  int PA = 0;
  int VPN = 0, tag = 0, index = 0, VPO = 0, PPN = 0;
//   Convert the incoming virtual address to a physical address. 
//     * if virt_address too large, 
//          log_entry(ILLEGALVIRTUAL,virt_address); 
//          return -1
  if(virt_address > 262143) {
    log_entry(ILLEGALVIRTUAL,virt_address);
    return -1;
  }
//    Break virtual address down into its parts
  VPO = virt_address & 0b111111111;
  VPN = (virt_address >> 9) & 0b111111111;
  index = (virt_address >> 9) & 0b1111;
  tag = (virt_address >> 13) & 0b11111;
  //     * if PPN is in the TLB, 
//	        compute PA 
//          log_entry(ADDRESS_FROM_TLB,PA);
//          return PA
  if((arr_tlb[index].valid == 1) && (arr_tlb[index].tag == tag)) {
    PPN = arr_tlb[index].ppn & 0b11111111111;
    PA = (PPN << 9) | VPO;
    log_entry(ADDRESS_FROM_TLB,PA);
        return PA;
  }
//    * else use the page table function to get the PPN:
//          * if VPN is in the Page Table
//	          compute PA 
//                  add the VPN and PPN in the TLB
//	          log_entry(ADDRESS_FROM_PAGETABLE,PA);
//	          return PA
  else if(arr_pt[VPN].valid == 1) {
    PPN = arr_pt[VPN].ppn & 0b11111111111;
    PA = (PPN << 9) | VPO;
// update tlb
    arr_tlb[index].valid = 1;
    arr_tlb[index].tag = tag;
    arr_tlb[index].ppn = arr_pt[VPN].ppn;
    log_entry(ADDRESS_FROM_PAGETABLE,PA);
        return PA;
  }
//	  * else load the frame into the page table
//	          PPN = load_frame(VPN) // use this provided library function
//                  add the VPN and PPN in to the page table
//                  add the VPN and PPN in to the TLB
// 		  compute PA
//		  log_entry(ADDRESS_FROM_PAGE_FAULT_HANDLER,PA);
// 		  return PA
  else{
    PPN = load_frame(VPN);
    PA = (PPN << 9) | VPO;
// update tlb
    arr_tlb[index].valid = 1;
    arr_tlb[index].tag = tag;
    arr_tlb[index].ppn = PPN;
// update pt
    arr_pt[VPN].valid = 1;
    arr_pt[VPN].ppn = PPN;
    log_entry(ADDRESS_FROM_PAGE_FAULT_HANDLER,PA);
        return PA;
  }  

}

char
get_byte(int phys_address) {
  char byte = 0;
  int tag = 0, index = 0, offset = 0, litE = 0, bigE = 0; 
  int turn1 = 0, turn2 = 0;
//  Break physical address down into its parts  
  offset = phys_address & 0b11;
  index = (phys_address >> 2) & 0b11111;
  tag = (phys_address >> 7) & 0b1111111111111;
    printf("VA,offset: %d\n",offset);
  printf("VA,index: %d\n",index);
  printf("VA,tag: %d\n",tag);
//  Use the incoming physical address to find the relevant byte. 
//    * if data is in the cache, use the offset (last 2 bits of address)
//          to compute the byte to be returned data
//          log_entry(DATA_FROM_CACHE,byte);
//          return byte 
  if(((arr_cache[index].valid1 == 1) && (arr_cache[index].tag1 == tag)) ||
      ((arr_cache[index].valid2 == 1) && (arr_cache[index].tag2 == tag))) {
    if(turn1 == 0){
      byte = (arr_cache[index].data1 >> (8*(3 - offset))) & 0b11111111;
    }
    else{
      byte = (arr_cache[index].data2 >> (8*(3 - offset))) & 0b11111111;   
    }
    log_entry(DATA_FROM_CACHE,byte);
    return byte;
  }
//     * else use the function get_word(phys_address) to get the 
//          4 bytes of data where the relevant byte will be at the
//          given offset (last 2 bits of address)
//          log_entry(DATA_FROM_MEMORY,byte);
//          return byte
  else {
//   convert data from little Endian to big Endian
    bigE = get_word(phys_address);
    litE = ((bigE & 0xff) << 24) | ((bigE & 0xff00) << 8) | ((bigE & 0xff0000) >> 8) | ((bigE & 0xff000000) >> 24);
    if((turn1 == 0 && turn2 ==0) || (turn1 == 1 && turn2 == 0)) {
      turn1 = 0; turn2 = 1;
      arr_cache[index].valid1 = 1;
      arr_cache[index].tag1 = tag;
      arr_cache[index].data1 = litE;
      byte = (arr_cache[index].data1 >> (8*(3 - offset))) & 0b11111111;
      }
    else {
      turn2 = 0; turn1 = 1;
      arr_cache[index].valid2 = 1;
      arr_cache[index].tag2 = tag;
      arr_cache[index].data2 = litE;
      byte = (arr_cache[index].data2 >> (8*(3 - offset))) & 0b11111111;
    }
    log_entry(DATA_FROM_MEMORY,byte);
    return byte;
  }
//NOTE: if the incoming physical address is too large, there is an
//error in the way you are computing it...
  return byte;
}

