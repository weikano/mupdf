/**
 * 
    gcc -I/usr/local/include -o fz_structure_text_example \
	/home/weikan/git/mupdf-android-fitz/libmupdf/docs/examples/fz_structure_text_example.c \
	/usr/local/lib/libmupdf.a \
	/usr/local/lib/libmupdf-third.a \
	-lm
./fz_structure_text_example document.pdf pageno
*/



#include <mupdf/fitz.h>

#include <stdio.h>
#include <locale.h>
#include <wchar.h>

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    char* input = argv[1];
    int page_number ;
    page_number = atoi(argv[2] - 1);

    fz_context *ctx;
    fz_document *doc;
    fz_page* page;

    ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
    fz_try(ctx)
    {
        fz_register_document_handlers(ctx);
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }

    fz_try(ctx)
    {
        doc = fz_open_document(ctx, input);
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }
    fz_stext_page* stext = NULL;
    fz_try(ctx)
    {
        fz_stext_options options = {
            0,
            1.0f
        };
        page = fz_load_page(ctx, doc, page_number);
        stext = fz_new_stext_page_from_page(ctx, page, &options);
        // fz_page* page = fz_load_page(ctx, doc, page_number);
        // fz_stext_page* stext = fz_new_stext_page_from_page(ctx, page, &options);

        if(stext->first_block == NULL) {
            printf("Structure has no blocks to walk\n");
            return 0;
        }
        fz_stext_block * block = NULL;
        fz_stext_line* line = NULL;
        fz_stext_char* ch = NULL;

        for(block = stext->first_block; block; block = block->next) {
            printf("=====================begin block===============\n");
            if(block->type == FZ_STEXT_BLOCK_IMAGE) {
                printf("image found with rect(%f,%f,%f,%f), transform(%f,%f,%f,%f,%f,%f)\n", 
                block->bbox.x0, block->bbox.y0, block->bbox.x1, block->bbox.y1, 
                block->u.i.transform.a, 
                block->u.i.transform.b, 
                block->u.i.transform.c, 
                block->u.i.transform.d, 
                block->u.i.transform.e, 
                block->u.i.transform.f
                );
            }else if(block->type == FZ_STEXT_BLOCK_TEXT) {
                for(line = block->u.t.first_line;line;line=line->next) {
                    printf("TextLine wmode:%d, rect(%f,%f, %f,%f)\n", line->wmode, line->bbox.x0, line->bbox.y0, line->bbox.x1, line->bbox.y1);
                    printf("========================start textline=======================\n");
                    for(ch = line->first_char;ch;ch=ch->next) {                
                        fz_font* font = ch->font;
                        fz_quad quad = ch->quad;
                        fz_rect rect = fz_rect_from_quad(quad);                        
                        wchar_t single_char[2] ={0};
                        single_char[0] = ch->c;                        
                        wprintf(L"ch:%lc", single_char);
                        printf(", fontname:%s, font bbox(%f,%f,%f,%f)\n", font->name, rect.x0, rect.y0, rect.x1, rect.y1);
                        
                    }
                    printf("========================end textline==========================\n");
                }
            }
            printf("=====================end block===============\n");
        }                
    }
    fz_catch(ctx)
    {

    }
    fz_drop_page(ctx, page);
    fz_drop_stext_page(ctx,stext);
    fz_drop_document(ctx,doc);
    fz_drop_context(ctx);            
    
    
    return 0;
}