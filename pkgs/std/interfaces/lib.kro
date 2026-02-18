

module mem {

    interface Copy {
        fn clone(self) -> self;
    }

    interface ShallowCopy {
        // Raw memory copy, will crash;
        fn copy(self) -> self;
    }

    /// A pointer without a reference count
    class Borrowed {
       var ptr: *T;
    }
    
    interface RawCopy {

    }

    class <T,Alloc> slice requires Alloc derives Allocator{
        var ptr: Borrowed<T>, 
            len: Alloc::Size;
    }

    /// 
    interface Delete {
       op delete(self)
    }

    // allocator?
    // fn forget(self) ->
    
    // TODO: Exported by prelude.
    op delete(mem:*T) with if <T> derives Delete {
        return delete(mem);
    }

}

module io {
    
}




module sync {

    // Type contracts to dictate if its safe to go across thead bounds

    interface Sync;

    interface Send;

}

module sync::lock {

    class Mutex {}

    
    class FreeReadLock {}
    
}

/// I propose that types can be conditional to give them more power than just being aliases.
module contracts {

    type MoreThan requires <const RHS:size,Then, const LHS:size, Otherwise> = if RHS > LSH: Then else Otherwise;

    /// as we need a way to stipulate that all things should implement sync or not sync

    

}


module sync::channel {

    interface Sender {

    }

    interface Receiver {

        fn self() -> {}


        fn receive
    }


    




    interface Channel requires <Sender, Receiver> {
        
    }




    class MpSc {}

    class MpSc {}

    class SpMc {}




}

/* ------------------------------ core/ptr.kro ------------------------------ */
module core::ptr {

    // A borrow checked pointer.
    class <T> Unique {
        ptr: *unsafe T;
    }

    class <T,Inc> Shared requires Inc derives Count{
        ptr:Unique<T>,

    }  
    
    
    class <T> Shared {
        ptr:Unique<T>,
    }  


    class <Then> Cow



    const NULL = 0 as unsafe *usize;
}
/* end -------------------------- core/ptr.kro -------------------------- end */

class <T      > list  ;
class <T, Size> array ;
class <T      > vec   ;

class <C = char, Container = collection::vec> string derives String {
    var container:Container<C>;






};




interface String {







}



class map    requires <



module core::prelude {
    pub import ::number::endian::native::*;
    pub import ::str::*;
    
    pub type LinkedList = ::collection::LinkedList;
    pub type Vector     = ::collection::Vector;
    pub type String<C, Alloc> = ::string::String<::collection::Vec>;    
    
    

    /*
        pub import ::str::String;
    */


}




module primitive {
    import ::char::Char;

    class <C> String requires C derives Char{

        type Char = Char;
        

    }

    interface Char {

    }
}

module format {
    
    interface Debug {

    }

    interface Display {
    }

}

module sequence {

    class sequence<T> {

    }

}



module hash {
    
    interface Hash {
        fn hash(self, hasher: *Hasher);
    }

    interface Hasher {
        fn finish(self) -> u64;

        fn write(self, value: T) requires <T> ;
    }
}

module hash {

    interface CastableAsInt {
        fn op as (self)[as_int] -> int;
    }
    
    interface Hasher {
        fn self(seed: int = 0);
        fn write(self, val: T) requires <T> if T impl As::<slice::<byte>>;
    }
    
    interface Hash {
        fn hash(self, hasher: *Hasher);
    }

    class StandardHasher extends Hasher {
        var hash: int;

        fn Hasher(seed: int = 0) {
            hash = seed;
        }
        
        fn write(self, val: T) requires <T> if T impl As::<slice::<byte>> {
            hash += (self.hash * 31) ^ (val as int);
        }
    }
}


fn main() -> i32 {
    
}


/// Maps one type to another
interface Transform requires <T,U,E> {
    fn apply(self, f: fn (*T) -> E?);
}

extend T requires <T,U,E> derives Transform 
    if T derives Iterator<U> {
    /// Iterates in place and applies the transform
    fn apply(self, func: fn (*T) -> E?) R {
        
        for i in self {
            func(i)    
        }
    } 
} 


module collection::list {
    class list extend Map {
        var data: list<int>;
        fn map(self, f: fn (T) -> T) {
            for i in data {
                data[i] = f(data[i]);
            }
        }
    }
}


module math {

    interface Min requires <T = self> {
        fn min (self) -> T;
    }

    interface Max requires <T = self> {
        fn max(self) -> T;
    }

    interface Floor {
        fn floor(self) -> self
    }

    interface Ceil {
        fn ceil(self) -> self
    }

    class <N,D=N> Ratio {

    }

}
    // size is non zero positive digit
module binary {

    class bool<SIZE:size = 1> {
        var inner: byte<SIZE>;
    }

}

module math::complex {

}

module number {
    
}

module number::endian {
    import little::*;
    import big::*;
    import native::*;
}










module future {

}



import mem::Clone;


class IndexOutOfBounds {}





interface Span {

}






interface Error if self impl Debug {}


/// Guarantees that a T is stored sequentially 
interface Sequential requires <T> {}

interface Allocator {
    // The max number of items that can be allocated at once.
    type Size = size;
}


interface Collection 
    requires <Key, Val, Alloc> 
    if Alloc impl Allocator & self impl Index<Key,Value>
    {

    
 
    
    fn op in (key:Key)[contains] -> bool;

    
    /// Gets without checking
    fn op [] unsafe (self, key: Key)[get] -> *Val;
    
    /// Returns the value at the location if already found
    fn insert(self, key:Key, value:Val) -> Val?;

    fn remove(self, key:Key) -> Val?;

    /// Gets the length of the collection 
    fn len(self) -> Alloc::Size;
    
    fn contains(key: Key) -> bool;

    fn len(self) -> Alloc::Size;

}

interface Record requires <Key, Val> extend Collection<Key, Val> {
    type Entry = (Key, Val);
    
    fn keys(self)            -> yield Key;
    fn values(self)          -> yield Val;
    fn entries(self)         -> yield Entry;
    fn entry(self, key: Key) -> Entry;
    fn extend(self, iter: yield Entry);
}

interface Generator requires <T> {
    
    fn op ++ (self)[next] -> yield T

}

extend with T if T impl Contains {

    op in fn next (self) -> yield T {
        var current = self.first;

        while current? {
            yield current;
            current += self.step;
        }
    }

}

interface Reverse {

}

interface Set {

    fn union(self) -> self;

    fn intersection(self) ->self;

}


struct Function requires <Args,Return> {

    type Args = Args;
    type Return = Return;

    var ptr: fn (Args...) -> Return;

}

interface Context requires <Fn> if Fn derives Function  {


    fn op as (self, action: Action) -> Return;

}

interface Any {}

/// if a type is ordered 
interface Ordered  {}

interface Resizable<T,Size> {
    fn reserve(self, additional: Size);
    
    fn shrink_to_fit(self) -> yield T;
}

interface Reversible<T> {
    fn reverse(self) -> yield T;
    fn reverse(self) -> self;
}

interface Compare {
    const fn compare(self)
}




/// Last In, First Out (LIFO)
interface Stack requires <T,Err=Error> if Err derives Error {

    fn push(self, value:T) -> Err?;

    fn pop(self) -> T?;

}

/// First In, First Out (FIFO)
interface Queue requires <T,Err=Error> if Err derives Error {

    fn enqueue(self, value:T) -> Err?;

    fn dequeue(self) -> T?;
}
 

interface PriorityQueue<T, P> {
    fn enqueue(self, value: (T,P)) -> Err?;

    fn dequeue(self) -> T?;    
}




interface Index requires <Idx>  {

    /// The type to return
    type Output;

    /// Returns a mutable pointer to the output
    op [] at(self, idx:  Idx) -> *Output?;        
    op [] at(self, idx: *Idx) -> *Output?;    
    
    /// Returns a const reference to the output
    const op [] at(self, idx:  const Idx) -> *const Output?;
    const op [] at(self, idx: *const Idx) -> *const Output?;
    const op [] at(self, idx: *const Idx) -> *const Output?;

    fn index_of(value:Output) -> Idx::Size; 
        if Idx impl Size;



    /// Returns an owned value cloned from the reference 
    const op [] at(self, idx:Idx) -> Output? if Output impl Copy {

        var out = self.index(index);

        // If the item exists at the index return the clone
        if out? {
            return out.clone();
        }

        // panic the error
        panic out;
    }
}


module core::collections {

    /// Represents a container that can check membership.
    interface Container<T> {
        fn op in (self, item: T)[contains] -> bool;
    }

    /// Represents a hashable object.
    interface Hashable {
        fn hash(self) -> u64;
    }

    /// Represents objects with a length.
    interface Sized {
        fn len(self) -> usize;
    }

    /// Represents callable objects.
    interface Callable<Args..., Return> {
        fn call(self, args: Args...) -> Return;
    }

    /// Represents an iterable object.
    interface Iterable<T> {
        fn op for (self)[iter] -> yield T;
    }

    /// Represents an iterator object.
    interface Iterator<T>: Iterable<T> {
        fn next(self) -> T?;
    }

    /// Represents a reversible iterable.
    interface Reversible<T>: Iterable<T> {
        fn reverse(self) -> yield T;
    }

    /// Represents a generator with additional methods.
    interface Generator<T>: Iterator<T> {
        fn send(self, value: T);
        fn throw(self, error: Error);
        fn close(self);
    }

    /// Represents a generic collection.
    interface Collection<T>: Sized, Iterable<T>, Container<T> {}

    /// Represents a sequence collection.
    interface Sequence<T>: Reversible<T>, Collection<T> {
        fn get(self, index: usize) -> T;
        fn index(self, value: T) -> usize;
        fn count(self, value: T) -> usize;
    }

    /// Represents a mutable sequence.
    interface MutableSequence<T>: Sequence<T> {
        fn set(self, index: usize, value: T);
        fn del(self, index: usize);
        fn insert(self, index: usize, value: T);
        fn append(self, value: T);
        fn clear(self);
        fn extend(self, other: yield T);
        fn pop(self) -> T;
        fn remove(self, value: T);
    }

    /// Represents a set-like collection.
    interface Set<T>: Collection<T> {
        fn union(self, other: Set<T>) -> Set<T>;
        fn intersection(self, other: Set<T>) -> Set<T>;
        }

    /// Represents a mutable set.
    interface MutableSet<T>: Set<T> {
        fn add(self, value: T);
        fn discard(self, value: T);
        fn clear(self);
    }

    /// Represents a mapping of keys to values.
    interface Mapping<Key, Value>: Collection<Key> {
        fn get(self, key: Key) -> Value?;
        fn keys(self) -> yield Key;
        fn values(self) -> yield Value;
        fn items(self) -> yield (Key, Value);
    }

    /// Represents a mutable mapping.
    interface MutableMapping<Key, Value>: Mapping<Key, Value> {
        fn set(self, key: Key, value: Value);
        fn del(self, key: Key);
        fn pop(self, key: Key) -> Value?;
        fn popitem(self) -> (Key, Value)?;
        fn clear(self);
        fn update(self, other: Mapping<Key, Value>);
        fn setdefault(self, key: Key, default: Value) -> Value;
    }

    /// Represents views over a mapping's keys, values, or items.
    interface MappingView<T>: Sized, Iterable<T> {}

    /// Represents a view over mapping items.
    interface ItemsView<Key, Value>: MappingView<(Key, Value)>, Set<(Key, Value)> {}

    /// Represents a view over mapping keys.
    interface KeysView<Key>: MappingView<Key>, Set<Key> {}

    /// Represents a view over mapping values.
    interface ValuesView<Value>: MappingView<Value>, Collection<Value> {}

    /// Represents an awaitable object.
    interface Awaitable {
        fn await(self) -> Any;
    }

    /// Represents a coroutine.
    interface Coroutine: Awaitable {
        fn send(self, value: Any);
        fn throw(self, error: Error);
        fn close(self);
    }

    /// Represents an asynchronous iterable.
    interface AsyncIterable<T> {
        fn aiter(self) -> yield T;
    }

    /// Represents an asynchronous iterator.
    interface AsyncIterator<T>: AsyncIterable<T> {
        fn anext(self) -> T?;
    }

    /// Represents an asynchronous generator.
    interface AsyncGenerator<T>: AsyncIterator<T> {
        fn asend(self, value: T);
        fn athrow(self, error: Error);
        fn aclose(self);
    }

    /// Represents a buffer.
    interface Buffer {
        fn buffer(self) -> *u8;
    }
}
