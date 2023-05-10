import hashlib

class SearchableEncryption:
    def __init__(self):
        self.index = {}  # The 'forward index'
        self.data = {}  # The 'encrypted data'
        self.keys = {}  # The 'trapdoors' (i.e., keys)

    def encrypt(self, id, text):
        words = text.split()
        encrypted_words = []

        for word in words:
            # Simple 'encryption' by hashing the word
            encrypted_word = hashlib.sha256(word.encode()).hexdigest()
            encrypted_words.append(encrypted_word)

        # Store the encrypted data
        self.data[id] = ' '.join(encrypted_words)

        # Add the encrypted words to the index under the document ID
        self.index[id] = encrypted_words

    def generate_trapdoor(self, word):
        # The 'trapdoor' is simply the encrypted version of the word
        self.keys[word] = hashlib.sha256(word.encode()).hexdigest()

    def search(self, word):
        # Use the 'trapdoor' to search the index and find document IDs
        if word in self.keys:
            encrypted_word = self.keys[word]
            return [id for id, words in self.index.items() if encrypted_word in words]
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