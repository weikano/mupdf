/**
 * 
    gcc -I/usr/local/include -o fz_annot_add \
	/home/weikan/git/mupdf-android-fitz/libmupdf/docs/examples/fz_annot_add.c \
	/usr/local/lib/libmupdf.a \
	/usr/local/lib/libmupdf-third.a \
	-lm
./fz_annot_add document.pdf
*/



#include <mupdf/fitz.h>
#include <mupdf/pdf/document.h>
#include <mupdf/pdf/annot.h>
#include <mupdf/pdf/crypt.h>

#include <stdio.h>
#include <time.h>

int main(int argc, char** argv)
{
    char* input = argv[1];

    fz_context *ctx;
    fz_document *doc;

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

    fz_try(ctx)
    {
        pdf_document *pdfdoc = pdf_document_from_fz_document(ctx, doc);
        printf("load pdfdoc\n");
        fz_page *page = fz_load_page(ctx, doc, 0);  
        printf("load pdfpage\n");
        pdf_page *pdfpage = pdf_page_from_fz_page(ctx, page);
        fz_stext_options options;
        printf("load stextpage\n");
        fz_stext_page *stext = fz_new_stext_page_from_page(ctx, page, &options);
        fz_stext_block *block = NULL;
        fz_stext_line *line = NULL;
        int lineCount = 0;
        float color[3] = {0,0,0};
        int quadpoint_count = 0;
        fz_quad quadpoints[10];
        for(block = stext->first_block;block; block = block->next) {
            printf("iterate block\n");
            if(block->type == FZ_STEXT_BLOCK_TEXT) {
                printf("iterate text block\n");
                for(line = block->u.t.first_line; line; line = line->next) {
                    // printf("iterate text line %d\n", lineCount);                    
                    quadpoints[quadpoint_count] = fz_quad_from_rect(line->bbox);
                    quadpoint_count ++;
                    // if(lineCount == 0) {
                    //     printf("addAnnot %d\n", lineCount);
                    //     pdf_annot* annot = pdf_create_annot(ctx, pdfpage, PDF_ANNOT_UNDERLINE);
                    //     printf("create annot %d\n", lineCount);
                    //     int set = 0;
                    //     time_t timep;
                    //     set = time(&timep);
                    //     pdf_set_annot_author(ctx, annot, "ireader");
                    //     printf("pdf_set_annot_author %d\n", lineCount);
                    //     pdf_set_annot_contents(ctx, annot, "mupdf add annnot");
                    //     printf("pdf_set_annot_contents %d\n", lineCount);
                    //     pdf_set_annot_creation_date(ctx, annot, set);
                    //     printf("pdf_set_annot_creation_date %d\n", lineCount);
                    //     //设置颜色，3代表rgb，其中color是0-255，黑色是{0,0,0}
                    //     pdf_set_annot_color(ctx, annot, 3, color);
                    //     printf("pdf_set_annot_color %d\n", lineCount);
                    //     fz_quad quad = fz_quad_from_rect(line->bbox);
                    //     printf("fz_quad_from_rect %d\n", lineCount);
                    //     pdf_set_annot_quad_points(ctx, annot, 1, &quad);
                    //     // pdf_set_annot_rect(ctx, annot, line->bbox);                        
                    //     printf("pdf_set_annot_rect %d\n", lineCount);
                    // }
                    
                    lineCount ++;
                }
            }
        }
        printf("after iterator\n");
        printf("addAnnot %d\n", lineCount);
        pdf_annot* annot = pdf_create_annot(ctx, pdfpage, PDF_ANNOT_UNDERLINE);
        printf("create annot %d\n", lineCount);
        int set = 0;
        time_t timep;
        set = time(&timep);
        pdf_set_annot_author(ctx, annot, "ireader");
        printf("pdf_set_annot_author %d\n", lineCount);
        pdf_set_annot_contents(ctx, annot, "mupdf add annnot");
        printf("pdf_set_annot_contents %d\n", lineCount);
        pdf_set_annot_creation_date(ctx, annot, set);
        printf("pdf_set_annot_creation_date %d\n", lineCount);
        pdf_set_annot_color(ctx, annot, 3, color);
        printf("pdf_set_annot_color %d\n", lineCount);
        
        pdf_set_annot_quad_points(ctx, annot, quadpoint_count, quadpoints);
        //highlight/underline annot cannot set rect， throws exception
        // pdf_set_annot_rect(ctx, annot, line->bbox);                        
        printf("pdf_set_annot_quad_points %d\n", lineCount);
        if(pdf_update_page(ctx, pdfpage)) {
            printf("pdf page updated\n");
        }
        pdf_write_options pdfoptions = pdf_default_write_options;
        if(pdf_can_be_saved_incrementally(ctx, doc)) {
            pdfoptions.do_incremental = 1;
        }
        // if(pdf_can_be_saved_incrementally){
        //     pdfoptions.do_incremental = 1;       
        //     pdfoptions.do_linear = 0;     
        // }                     
        pdf_save_document(ctx, doc, "/home/weikan/resources/text_render_mode1_fix.pdf", &pdfoptions);
        
        fz_drop_page(ctx, page);    
                    
    }
    fz_catch(ctx)
    {    
        fz_log_error(ctx, fz_caught_message(ctx));
    }
    
    
    fz_drop_document(ctx,doc);
    fz_drop_context(ctx);        
    
    return 0;
}