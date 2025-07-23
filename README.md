# Implementing probabilistic data structures in C

## Bloom Filters

To implement a Bloom filter we need an auxilary data, structure which helps us manipulate bits. This data structure compactly stores bits.

## HyperLogLog

These tests will check for insertion into the HLL; counting the approximate number of elements; and merging two HLLs. The final test reads from a text file of phrases, one per line, with a 30% rate of repetition.

###  Tests
```bash
❯ make clean && make test && ./tests 24 "arjangt is my username" phrases.txt
rm -f *.o hll.a tests
Creating HLL with parameter `p`=24


******************************
Number of elements ~= 0.000000
Inserting `arjangt is my username` into HLL
Number of elements ~= 1.000000
Inserting 20.000000 more elements...
Non-zero registers: 21
Number of elements ~= 21.000013


******************************
Inserting `peanut butter` into HLL 1
Inserting `banana` into HLL 2
Merged count: ~2.00


******************************
Loaded 50000 sentences
Bloom filter Bitvector utilization: 0.05%
Inserted 50000 sentences (35000 unique)
Number of elements ~= 34986.454247
```

This was generated in Python using the following script.

```Python
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


## References
	* [Bloom Filter](https://en.wikipedia.org/wiki/Bloom*filter)
    * [HyperLogLog](https://en.wikipedia.org/wiki/HyperLogLog)
    * [Bit Array](https://en.wikipedia.org/wiki/Bit*array)
	* [Distinct Elements in Stream](https://arxiv.org/abs/2301.10191) by Chakraborty et al.
