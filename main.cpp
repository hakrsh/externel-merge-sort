#include <fstream>
#include <iostream>
#include <time.h>
using namespace std;

#define int long long
#define MAX 10000000 //100MB
#define LL_CHUNKSIZE 10000
int CHUNKSIZE;
int N_CHUNKS;

struct node {
  int val;
  int id;
  int offset;
  node *next;
  node(){};
  node(int data, int id_,int offset_) {
    val = data;
    id = id_;
    offset = offset_;
    next = nullptr;
  }
};

struct linkedlist {
  node *head, *tail;
  linkedlist() {
    head = nullptr;
    tail = nullptr;
  }
};
// get data from ith temp file
string get_data(ifstream in[], int i) {
  string data;
  in[i] >> data;
  return data;
}

void insert(node **head, node **tail, int val, int id, int offset) {
  if (!(*head)) {
    *head = new node(val, id,offset);
    *tail = *head;
  } else {
    (*tail)->next = new node(val, id,offset);
    *tail = (*tail)->next;
  }
}

void print(node *head) {
  if (head == nullptr) {
    cout << "EMPTY!\n";
    return;
  }
  auto temp = head;
  while (temp) {
    cout << temp->val << " ,";
    temp = temp->next;
  }
}
void del(node **head, node **tail) {
  while (*head) {
    node *temp = *head;
    *head = temp->next;
    delete temp;
  }
  *head = *tail = nullptr;
}
void refill(node **head, node **tail, int id, int offset,ifstream in[]) {
  // close if stream is bad
  if (in[id].peek() == EOF) {
    // cout << "bad stream\n";
    in[id].close();
    return;
  }

  int k = 0;
  while (k < LL_CHUNKSIZE && in[id].peek() != EOF) {
    string data = get_data(in, id);
    if (data == "") {
      break;
    }
    insert(head, tail, stoll(data), id,offset);
    k++;
    // i++;
  }
}
/* linked list END */
void heapify(node *a[], int i, int size) {
  int l = 2 * i;
  int r = 2 * i + 1;
  int smallest;
  if (l < size && a[l]->val < a[i]->val)
    smallest = l;
  else
    smallest = i;
  if (r < size && a[r]->val < a[smallest]->val)
    smallest = r;
  if (i != smallest) {
    auto temp = a[i];
    a[i] = a[smallest];
    a[smallest] = temp;
    heapify(a, smallest, size);
  }
}
node *extract_min(node *a[], int &size) {
  node *temp = a[0];
  a[0] = a[size - 1];
  size--;
  heapify(a, 0, size);
  return temp;
}
void insert_heap(node *a[], int &size, node *new_node) {
  int i = size;
  a[i] = new_node;
  while (i >= 0 && a[i / 2]->val > a[i]->val) {
    auto temp = a[i / 2];
    a[i / 2] = a[i];
    a[i] = temp;
    i /= 2;
  }
  size++;
}

/* merge sort */
void merge(int *a, int p, int q, int r) {
  int n = q - p + 1;
  int m = r - q;
  auto arr1 = new int[n];
  auto arr2 = new int[m];
  for (int i = 0; i < n; i++)
    arr1[i] = a[p + i];
  for (int i = 0; i < m; i++)
    arr2[i] = a[q + 1 + i];

  int i, j, k;
  i = j = 0;
  k = p;
  while (i < n && j < m) {
    if (arr1[i] <= arr2[j])
      a[k++] = arr1[i++];
    else
      a[k++] = arr2[j++];
  }
  while (i < n)
    a[k++] = arr1[i++];
  while (j < m)
    a[k++] = arr2[j++];
  delete[] arr1;
  delete[] arr2;
}
void mergesort(int a[], int p, int r) {
  int q;
  if (p < r) {
    q = (p + r) / 2;
    mergesort(a, p, q);
    mergesort(a, q + 1, r);
    merge(a, p, q, r);
  }
}
/* sort ends */

int32_t main(int32_t argc, char *argv[]) {
  // cout << "Enter file name: ";
  // string fname;
  // cin >> fname;
  if (argc < 3) {
    cout << "ERROR: USAGE <inputfile> <outputfile>\n";
    exit(0);
  }
  string inputFile = argv[1];
  string outputFile = argv[2];
  string tempFile;
  // calculte runtime
  clock_t start = clock();
  ifstream inFile;
  inFile.open(inputFile);
  // check if file exists
  if (!inFile) {
    cout << "File not found\n";
    exit(0);
  }
  string temp;
  // cout total elements
  cout << "calculating number of elements...\n";
  long long total_elements = 0;
  while (getline(inFile, temp, ',')) {
    total_elements++;
  }
  inFile.close();
  cout << "Total elements: " << total_elements << endl;
  CHUNKSIZE = total_elements > MAX ? MAX : total_elements;
  N_CHUNKS = total_elements / CHUNKSIZE;
  if (total_elements % CHUNKSIZE != 0)
    N_CHUNKS++;
  cout << "N_CHUNKS: " << N_CHUNKS << endl;
  cout << "time taken: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
  cout << "creating sorted temp files...\n";

  int c = 0;
  int i = 0;
  // int chunk[CHUNKSIZE];
  int *chunk = new int[CHUNKSIZE];
  inFile.open(inputFile);
  while (getline(inFile, temp, ',')) {
    if (c == CHUNKSIZE) {
      mergesort(chunk, 0, CHUNKSIZE - 1);
      ofstream out;
      tempFile = "temp" + to_string(i++) + ".txt";
      out.open(tempFile);
      for (int j = 0; j < CHUNKSIZE; j++) {
        out << chunk[j] << " ";
      }
      c = 0;
      out.close();
    }
    if (temp == "") {
      break;
    }
    chunk[c++] = stoll(temp);
  }
  // write last chunk
  if (c > 0) {
    mergesort(chunk, 0, c - 1);
    ofstream out;
    tempFile = "temp" + to_string(i++) + ".txt";
    out.open(tempFile);
    for (int j = 0; j < c; j++) {
      out << chunk[j] << " ";
    }
    out.close();
  }
  inFile.close();
  delete[] chunk;
  /* temp file ends */
  cout << "time taken: " << double(clock() - start) / CLOCKS_PER_SEC << endl;
  // if (N_CHUNKS == 1) {
  //   cout << "No need to merge\n";
  //   // copy temp file to output file
  //   std::ifstream src(tempFile, std::ios::binary);
  //   std::ofstream dst(outputFile, std::ios::binary);
  //   dst << src.rdbuf();
  //   src.close();
  //   dst.close();
  //   remove(tempFile.c_str());
  //   return 0;
  // }
  cout << "merging files...\n";
  // open all temp files
  // ifstream in[N_CHUNKS];
  ifstream *in = new ifstream[N_CHUNKS];
  for (int i = 0; i < N_CHUNKS; i++) {
    tempFile = "temp" + to_string(i) + ".txt";
    in[i].open(tempFile);
  }

  // inital filling; offset = 0
  // linkedlist ll[N_CHUNKS];
  linkedlist *ll = new linkedlist[N_CHUNKS];
  for (int j = 0; j < N_CHUNKS; j++)
    for (int i = 0; i < LL_CHUNKSIZE; i++) {
      auto data = get_data(in, j);
      if (data == "")
        continue;
      insert(&(ll[j].head), &(ll[j].tail), stoll(data),
             j,0); // insert into linked list
    }

  // Final sorted file
  ofstream out;
  // fname = "/home/hari/testing/aps/A4/out/output.txt";
  out.open(outputFile);

  // node *heap[N_CHUNKS];
  auto heap = new node *[N_CHUNKS];
  for (int i = 0; i < N_CHUNKS; i++)
    heap[i] = ll[i].head;
  int heapsize = N_CHUNKS;
  for (int i = heapsize / 2; i >= 0; i--)
    heapify(heap, i, heapsize);
  int refill_count = 0;
  while (heapsize) {
    auto temp = extract_min(heap, heapsize);
    int min_ = temp->val;
    if (temp->next)
      insert_heap(heap, heapsize, temp->next);
    // refill
    else {
      int id = temp->id;
      int offset = temp->offset;
      del(&(ll[id].head), &(ll[id].tail));
      refill(&(ll[id].head), &(ll[id].tail), id, offset+1,in);
      refill_count++;
      if (ll[id].head)
        insert_heap(heap, heapsize, ll[id].head);
    }
    out << min_ << ",";
  }
  cout << "refill count: " << refill_count << endl;
  out.close();
  delete[] heap;
  delete[] ll;
  for (int i = 0; i < N_CHUNKS; i++)
    in[i].close();
  delete[] in;
  cout << "written sorted data to " << outputFile << endl;
  cout << "time taken: " << double(clock() - start) / CLOCKS_PER_SEC << endl;
  // delete all temp files
  cout << "deleting temp files...\n";
  for (int i = 0; i < N_CHUNKS; i++) {
    tempFile = "temp" + to_string(i) + ".txt";
    remove(tempFile.c_str());
  }
  cout << "time taken: " << double(clock() - start) / CLOCKS_PER_SEC << endl;
  return 0;
}
