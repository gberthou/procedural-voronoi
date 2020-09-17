class ChunkManager:
    def __init__(self, chunk_generator, chunk_database):
        self.chunk_generator = chunk_generator
        self.chunk_database = chunk_database
        self.loaded_chunks = dict()

    def load_chunk(self, key):
        # 1. Chunk already loaded
        if key in self.loaded_chunks.keys():
            #print("Found key", key, "in cache!")
            return False, self.loaded_chunks[key]

        # 2. Chunk not loaded but present in database
        chunk = self.chunk_database.load(key)
        if chunk != None:
            #print("Found key", key, "in database!")
            self.loaded_chunks[key] = chunk
            return True, chunk
        
        # 3. Chunk nor loaded nor in database, create a new one
        #print("Key", key, "not found, must generate a new one!")
        chunk = self.chunk_generator.new_chunk(key)
        self.chunk_database.save(key, chunk)
        self.loaded_chunks[key] = chunk
        return True, chunk

    def unload_chunk(self, key):
        if not key in self.loaded_chunks.keys():
            return

        #print("Unloaded key", key, "!")
        # Optional (not implemented yet): save changes to database, if any change occurred

        chunk = self.loaded_chunks[key]
        del self.loaded_chunks[key]
        return chunk

    def keep_only_chunks(self, keys):
        for key in self.loaded_chunks.keys() - keys:
            self.unload_chunk(key)
        #print(len(self.loaded_chunks.keys()), "living chunks!")