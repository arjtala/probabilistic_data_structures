# Implementing probabilistic data structures in C

## Utilities

To begin with, we need a data structure which helps us store and manipulate bits, the [Bit Array](https://en.wikipedia.org/wiki/Bit*array):

```C
typedef struct {
	uint64_t *data;
	size_t size;
} BitVector;
```

This struct contains the actual byte data and a `size` parameter, which determines the number of elements. To track the number of bits per unit, we define `BITS_IN_TYPE`, which multiplies the size of a given `type` by [`CHAR_BIT`](https://en.cppreference.com/w/cpp/types/climits.html) (the number of bits in char, which these days, is 8 bits for almost all architectures but some older machines used to have a 7-bit byte). We just need the ability to get, set, flip, and clear any given bit, and a few helper utilities to print and convert between binary representation.

One important function is the `msb_position`, which finds the most significant bit using [`__builtin_ctzll`](https://www.geeksforgeeks.org/c/builtin-functions-gcc-compiler/), i.e. the first non-zero element when counting from the left. This is useful later for the Hyperloglog data structure.

Another important concept here is hashing. We could, of course, use a standard 64-bit implementation such as the [Fowler–Noll–Vo](https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash):

```C
uint64_t hash_64(const void *buff, size_t len) {
	uint64_t seed = 14695981039346656037ULL;
	uint64_t prime = 1099511628211ULL;
    const uint8_t *data = buff;
    uint64_t h = seed;
    for (size_t i = 0; i < len; ++i) {
        h ^= data[i];
        h *= prime;
    }
    return h;
}
```

Here, we start with a seed value. Then for each byte in the data to be hashed, we XOR it into the current hash, and then multiply the result by a prime number to "mix" the bits. This isn't cryptographically secure, but quite fast and has decent uniformity (distributing input values relatively well).

We do want to avoid collisions, so while still retaining performance and sacrificing cryptographic security, we can use a 64-bit [MurmurHash](https://en.wikipedia.org/wiki/MurmurHash), and in fact we can use it with two different seeds to help improve the data distribution--in the case of a Bloom filter this helps reduce false positives.

## [Bloom Filter](https://en.wikipedia.org/wiki/Bloom*filter)

A Bloom filter is a space-efficient probabilistic data structure used to test whether an element is possibly in a set or definitely not.

To insert an item, we first compute `k` hash values of the item, and set the bits at those k positions in the bit vector. To check membership, we recompute the k hashes; if all the corresponding bits are set, the item might be in the set, and if any bit is not set, the item is definitely not in the set. Notice the term "may" in the first case--this is a probabilistic data structure and the false positive rate increases as more data is inserted. This happens because we have a fixed-size bit array and even with multiple hash functions we can end up with collisions in the bit-positions for different inputs.

for a Bloom filter with:
- \( n \) = number of inserted elements
- \( m \) = number of bits in the bit array
- \( k \) = number of hash functions

The approximate false positive probability, \( P_{fp} \), is given by:

$$FPR = \left(1 - e^{-kn/m}\right)^k$$

### Tests

To execute the tests, just build the binary:
```bash
make test && ./tests
```

Output:
```
******************************
[test_bitvector_display]
Built expected bitvector (allocated size = 8): [ 0 0 0 0 0 0 ]
Set position 3:
Expected: [ 0 0 0 1 0 0 ]
Result:   [ 0 0 0 1 0 0 ]
Flip position 5:
Expected: [ 0 0 0 1 0 1 ]
Result:   [ 0 0 0 1 0 1 ]
Clear position 3:
Expected: [ 0 0 0 0 0 1 ]
Result:   [ 0 0 0 0 0 1 ]


******************************
[test_bitvector_values]
Setting bit in position 5: 1 == 1
Clearing bit in position 5: 0 == 0


******************************
[test_bloom_filter]
Inserting value 9 into filter: 1 == 1
Checking existence of value `abc` in filter: 0 == 0
Inserting value `abc` into filter: 1 == 1
Hash: 417419622498 (2)[ 0 0 1 0 0 0 0 0 ]
Inserting 1000 more elements...
Bitvector utilization: 97.46%
```

## [HyperLogLog](https://en.wikipedia.org/wiki/HyperLogLog)

These tests will check for insertion into the HLL; counting the approximate number of elements; and merging two HLLs. The final test reads from a text file of phrases, one per line, with a 30% rate of repetition.

###  Tests

To execute the test, build the binary and pass in two arguments:
    * `p`: The size parameter, such that `m = 2^p`
	* `<text>`: Any data (e.g. text in quotes) that will be inserted in the datastructure
	* Input text file: This is the file that contains tens of thousands of phrases with some rate of duplication (see below for generating this).

```bash
make test && ./tests 24 "arjangt is my username" phrases.txt

```
Output:
```
******************************
[test_HLL]
Number of elements ~= 0.000000
Inserting `arjangt is my username` into HLL
Number of elements ~= 1.000000
Inserting 20.000000 more elements...
Non-zero registers: 21
Number of elements ~= 21.000013


******************************
[test_merge_two]
Inserting `peanut butter` into HLL 1
Inserting `banana` into HLL 2
Merged count: ~2.00


******************************
[test_hll_accuracy]
True count: 100000, HLL estimate: 99993.39
Relative error 0.0066%


******************************
[test_hll_duplicates]
Estimate with 100,000 duplicates of same string: 1.00


******************************
[test_batch_phrases]
Loaded 50000 sentences
Bloom filter Bitvector utilization: 0.05%
Inserted 50000 sentences (35000 unique)
Number of elements ~= 34986.454247
```

#### Generating phrases

This was generated in Python using the following script.

```python
import random

# Word pools for generating sensible phrases
adjectives: list[str] = [
    "bright", "dark", "quick", "slow", "happy", "sad", "large", "small",
    "old", "new", "hot", "cold", "loud", "quiet", "smooth", "rough",
    "clean", "dirty", "empty", "full", "light", "heavy", "sharp", "dull",
    "sweet", "bitter", "fresh", "stale", "strong", "weak", "deep", "shallow",
    "wide", "narrow", "high", "low", "fast", "calm", "busy", "lazy",
    "rich", "poor", "young", "ancient", "modern", "simple", "complex", "gentle"
]

nouns: list[str] = [
    "cat", "dog", "house", "car", "tree", "book", "phone", "computer",
    "chair", "table", "window", "door", "flower", "bird", "fish", "mountain",
    "river", "ocean", "cloud", "star", "moon", "sun", "road", "bridge",
    "garden", "forest", "city", "village", "school", "store", "park", "beach",
    "castle", "tower", "boat", "plane", "train", "bicycle", "camera", "guitar",
    "piano", "painting", "story", "dream", "journey", "adventure", "mystery", "secret"
]

verbs: list[str] = [
    "runs", "walks", "jumps", "flies", "swims", "dances", "sings", "laughs",
    "cries", "sleeps", "wakes", "eats", "drinks", "reads", "writes", "draws",
    "plays", "works", "rests", "thinks", "dreams", "travels", "explores", "discovers",
    "creates", "builds", "breaks", "fixes", "opens", "closes", "starts", "stops",
    "grows", "shrinks", "shines", "glows", "whispers", "shouts", "listens", "watches",
    "remembers", "forgets", "learns", "teaches", "helps", "protects", "celebrates", "mourns"
]

prepositions: list[str] = [
    "in", "on", "at", "by", "for", "with", "from", "to", "of", "about",
    "under", "over", "through", "between", "among", "during", "before", "after",
    "inside", "outside", "near", "far", "above", "below", "beside", "behind"
]

def generate_phrase(length):
    """
    Generate a sensible phrase with approximately the given number of words.
    """
    if length <= 2:
        # Simple noun + verb
        return f"The {random.choice(nouns)} {random.choice(verbs)}."
    elif length == 3:
        return f"The {random.choice(adjectives)} {random.choice(nouns)} {random.choice(verbs)}."
    elif length == 4:
        return f"A {random.choice(nouns)} {random.choice(verbs)} {random.choice(prepositions)} the {random.choice(nouns)}."
    elif length == 5:
        return f"The {random.choice(adjectives)} {random.choice(nouns)} {random.choice(verbs)} {random.choice(prepositions)} the {random.choice(adjectives)} {random.choice(nouns)}."
    else:
        # Compose longer phrase using a consistent pattern
        phrase = ["The", random.choice(adjectives), random.choice(nouns), random.choice(verbs)]
        for _ in range(length - 4):
            phrase.append(random.choice([random.choice(prepositions), random.choice(adjectives), random.choice(nouns)]))
        return " ".join(phrase).capitalize() + "."

def generate_phrases(
    total_phrases=10000,
    repeat_rate=0.05,
    min_phrase_length=3,
    max_phrase_length=6,
    output_file="sensible_phrases.txt"
) -> list[str]:
    assert 0 <= repeat_rate < 1, "Repeat rate must be between 0 and 1"
    assert 1 <= min_phrase_length <= max_phrase_length, "Phrase length range invalid"

    unique_count = int(total_phrases * (1 - repeat_rate))
    repeated_count = total_phrases - unique_count

    # Generate unique phrases
    unique_phrases = set()
    while len(unique_phrases) < unique_count:
        phrase_length = random.randint(min_phrase_length, max_phrase_length)
        phrase = generate_phrase(phrase_length)
        unique_phrases.add(phrase)
    unique_phrases = list(unique_phrases)

    # Add repeated ones
    repeated_phrases = random.choices(unique_phrases, k=repeated_count)

    # Combine and shuffle
    all_phrases = unique_phrases + repeated_phrases
    random.shuffle(all_phrases)

    print(f"Generated {total_phrases} phrases with ~{repeat_rate*100:.1f}% repeat rate.")
    return all_phrases

if __name__ == "__main__":
	# Generate phrases with default parameters

	# Example with custom parameters
	phrases = generate_phrases(
		total_phrases=50000,
		repeat_rate=0.3,
	)
	print(f"Total: {len(phrases)}, Unique: {len(set(phrases))}")
	# Display first 10 phrases as preview
	print("\nFirst 10 phrases:")
	for i, phrase in enumerate(phrases[:10], 1):
		print(f"{i:2d}. {phrase}")

	output_file: str = "phrases.txt"
	with open(output_file, "w") as f:
		for phrase in phrases:
			f.write(f"{phrase}\n")
```

```output
Generated 50000 phrases with ~30.0% repeat rate.
Total: 50000, Unique: 35000

First 10 phrases:
 1. A story runs far the secret.
 2. A guitar learns between the moon.
 3. The sweet bird teaches with light.
 4. The sharp journey protects by the deep star.
 5. The full bridge starts outside the full ocean.
 6. The large city remembers.
 7. The busy computer draws.
 8. The sharp mystery jumps during painting.
 9. The simple painting grows.
10. The fresh beach fixes.
```
