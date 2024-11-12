package lru

import (
	"container/heap"
	"time"
)

type entry struct {
	heapIdx        int
	lastAccessTime int64

	key []byte
	val any
}

type LRU struct {
	// 容量
	capacity int
	// 存储数据
	entries map[string]*entry
	// 堆
	heap lastAccessHeap
}

func NewLRU(capacity int) *LRU {
	return &LRU{
		capacity: capacity,
		entries:  make(map[string]*entry),
		heap:     make([]*entry, 0),
	}
}

func (l *LRU) Get(key []byte) (any, bool) {
	entry, ok := l.entries[string(key)]
	if !ok {
		return nil, false
	}

	cur := time.Now().Unix()
	if entry.lastAccessTime != cur {
		heap.Fix(&l.heap, entry.heapIdx)
	}

	return entry.val, true
}

func (l *LRU) Put(key []byte, val any) {
	e, ok := l.entries[string(key)]
	if ok {
		e.val = val
		cur := time.Now().Unix()
		if e.lastAccessTime != cur {
			heap.Fix(&l.heap, e.heapIdx)
		}
		return
	}

	if len(l.entries) >= l.capacity {
		oldest := heap.Pop(&l.heap).(*entry)
		delete(l.entries, string(oldest.key))
	}
	e = &entry{
		lastAccessTime: time.Now().Unix(),
		key:            key,
		val:            val,
	}
	l.entries[string(key)] = e
	heap.Push(&l.heap, e)
}

type lastAccessHeap []*entry

func (h *lastAccessHeap) Len() int {
	return len(*h)
}

func (h *lastAccessHeap) Less(i, j int) bool {
	hv := *h
	return hv[i].lastAccessTime < hv[j].lastAccessTime
}

func (h *lastAccessHeap) Swap(i, j int) {
	hv := *h
	hv[i], hv[j] = hv[j], hv[i]
	hv[i].heapIdx = i
	hv[j].heapIdx = j
}

func (h *lastAccessHeap) Push(x any) {
	e := x.(*entry)
	e.heapIdx = len(*h)
	oh := *h
	*h = append(oh, e)
}
func (h *lastAccessHeap) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	old[n-1] = nil // avoid memory leak
	*h = old[0 : n-1]
	return x
}
