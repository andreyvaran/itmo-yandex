#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace cls_06 {
    struct Document {
        std::string uri;
        std::string text;
    };

    using Corpus = std::vector<Document>;

    class CorpusIndex {
    public:
        CorpusIndex(const Corpus &corpus) {
            for (const Document &doc: corpus) {
                indexDocument(doc);
            }
        }

        std::vector<const Document*> find(std::string_view word) const {
            std::string word_str(word); // Преобразуем std::string_view в std::string
            if (index_.count(word_str) > 0) {
                std::vector<const Document*> result = index_.at(word_str);
                std::sort(result.begin(), result.end(), compareDocuments);
                return result;
            } else {
                return {};
            }
        }

    private:
        std::unordered_map<std::string, std::vector<const Document *>> index_;

        void indexDocument(const Document &doc) {
            std::string text = doc.text;
            std::transform(text.begin(), text.end(), text.begin(), ::tolower); // Приводим к нижнему регистру
            std::string_view text_view(text);
            size_t start = 0;

            while (start < text_view.size()) {
                size_t end = text_view.find_first_of(" \t\n\r.,;!?()[]{}", start);
                if (end == std::string::npos) {
                    end = text_view.size();
                }
                std::string_view word = text_view.substr(start, end - start);
                index_[std::string(word)].push_back(&doc);
                start = end + 1;
            }
        }

        static bool compareDocuments(const Document *doc1, const Document *doc2) {
            size_t count1 = std::count(doc1->text.begin(), doc1->text.end(), ' ');
            size_t count2 = std::count(doc2->text.begin(), doc2->text.end(), ' ');
            if (count1 > count2) {
                return true;
            } else if (count1 < count2) {
                return false;
            }

            return doc1->uri < doc2->uri;
        }
    };
}
