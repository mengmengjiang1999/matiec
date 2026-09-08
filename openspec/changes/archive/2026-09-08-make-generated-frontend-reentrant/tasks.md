## 1. Context-owned parser state

- [x] 1.1 Move frontend classification tables into `ParserState`
- [x] 1.2 Reset classification and transient parser state on context reuse

## 2. Generated frontend isolation

- [x] 2.1 Qualify handwritten scanner and grammar session state as thread-local
- [x] 2.2 Add a checked post-generation transformation for Flex/Bison globals
- [x] 2.3 Wire generated object dependencies into the Autotools build

## 3. Verification and documentation

- [x] 3.1 Add concurrent frontend state-isolation regression coverage
- [x] 3.2 Update README and architecture inventories
- [x] 3.3 Run normal and sanitizer suites plus strict OpenSpec validation

## 4. Completion

- [x] 4.1 Archive the OpenSpec change
- [x] 4.2 Commit and push the completed change
