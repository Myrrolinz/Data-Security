import hashlib
from collections import defaultdict

class SearchableEncryption:
    def __init__(self):
        self.index = defaultdict(list)  
        self.data = {}  # The 'encrypted data'
        self.keys = {}  # The 'trapdoors' (i.e., keys)

    def encrypt(self, id, text):
        words = text.split()
        encrypted_words = []

        for word in words:
            # Simple 'encryption' by hashing the word
            encrypted_word = hashlib.sha256(word.encode()).hexdigest()
            encrypted_words.append(encrypted_word)

            # Add the document ID to the index under the encrypted word
            self.index[encrypted_word].append(id)

        # Store the encrypted data
        self.data[id] = ' '.join(encrypted_words)

    def generate_trapdoor(self, word):
        # The 'trapdoor' is simply the encrypted version of the word
        self.keys[word] = hashlib.sha256(word.encode()).hexdigest()

    def search(self, word):
        # Use the 'trapdoor' to search the index and find document IDs
        if word in self.keys:
            return self.index[self.keys[word]]
        else:
            return []

    def decrypt(self, id):
        # 'Decrypt' the data by simply returning it - in a real system, this would not be possible without the decryption key
        return self.data[id]

# Usage:

se = SearchableEncryption()

# Encrypt some data
se.encrypt(1, "hello world")
se.encrypt(2, "goodbye world")

# Generate some trapdoors
se.generate_trapdoor("hello")
se.generate_trapdoor("world")

# Search for some data
print(se.search("hello"))  # [1]
print(se.search("world"))  # [1, 2]

# Decrypt some data
print(se.decrypt(1))  
print(se.decrypt(2))  