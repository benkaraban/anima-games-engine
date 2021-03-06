/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
��# i n c l u d e   < E P I / G U I / A p p l i c a t i o n W i n d o w B a s e . h >  
  
 # i n c l u d e   < E P I / G U I / G u i D o c u m e n t M a n a g e r . h >  
 # i n c l u d e   < Q A c t i o n >  
  
 n a m e s p a c e   E P I  
 {  
 / / - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 A p p l i c a t i o n W i n d o w B a s e : : A p p l i c a t i o n W i n d o w B a s e ( c o n s t   P t r < C o n t r o l l e r M a i n > &   p C t r l ,   Q W i d g e t   *   p a r e n t ,   Q t : : W i n d o w F l a g s   f l a g s   )   :  
         Q M a i n W i n d o w ( p a r e n t ,   f l a g s ) ,  
         _ p C t r l ( p C t r l ) ,  
         a c t i o n K e y P a d 0 ( N U L L ) ,  
         a c t i o n K e y P a d 1 ( N U L L ) ,  
         a c t i o n K e y P a d 2 ( N U L L ) ,  
         a c t i o n K e y P a d 3 ( N U L L ) ,  
         a c t i o n K e y P a d 4 ( N U L L ) ,  
         a c t i o n K e y P a d 5 ( N U L L ) ,  
         a c t i o n K e y P a d 6 ( N U L L ) ,  
         a c t i o n K e y P a d 7 ( N U L L ) ,  
         a c t i o n K e y P a d 8 ( N U L L ) ,  
         a c t i o n K e y P a d 9 ( N U L L ) ,  
         a c t i o n K e y 0 ( N U L L ) ,  
         a c t i o n K e y 1 ( N U L L ) ,  
         a c t i o n K e y 2 ( N U L L ) ,  
         a c t i o n K e y 3 ( N U L L ) ,  
         a c t i o n K e y 4 ( N U L L ) ,  
         a c t i o n K e y 5 ( N U L L ) ,  
         a c t i o n K e y 6 ( N U L L ) ,  
         a c t i o n K e y 7 ( N U L L ) ,  
         a c t i o n K e y 8 ( N U L L ) ,  
         a c t i o n K e y 9 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 0 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 1 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 2 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 3 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 4 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 5 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 6 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 7 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 8 ( N U L L ) ,  
         a c t i o n C t r l K e y P a d 9 ( N U L L ) ,  
         a c t i o n C t r l K e y 0 ( N U L L ) ,  
         a c t i o n C t r l K e y 1 ( N U L L ) ,  
         a c t i o n C t r l K e y 2 ( N U L L ) ,  
         a c t i o n C t r l K e y 3 ( N U L L ) ,  
         a c t i o n C t r l K e y 4 ( N U L L ) ,  
         a c t i o n C t r l K e y 5 ( N U L L ) ,  
         a c t i o n C t r l K e y 6 ( N U L L ) ,  
         a c t i o n C t r l K e y 7 ( N U L L ) ,  
         a c t i o n C t r l K e y 8 ( N U L L ) ,  
         a c t i o n C t r l K e y 9 ( N U L L )  
 {  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n D e l e t e C u r r e n t P t i e s ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n C o p y C u r r e n t P t i e s ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n P a s t e P t i e s C o p y ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t G u i D o c u m e n t D i a l o g N e x t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t G u i D o c u m e n t D i a l o g P r e v ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n U n d o C u r r e n t G u i D o c u m e n t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n R e d o C u r r e n t G u i D o c u m e n t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n P l a y S t o p C u r r e n t G u i D o c u m e n t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n R e s t a r t C u r r e n t G u i D o c u m e n t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S t o p C u r r e n t G u i D o c u m e n t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n E x e c u t e S c r i p t s C u r r e n t G u i D o c u m e n t ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n L o g D e b u g I n f o ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t M u l t i T r a n s f o r m V T o o l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t M o v e V T o o l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t R o t a t e V T o o l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t S c a l e V T o o l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t C o p y L o c a t i o n V T o o l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t C o p y C a m e r a L o c a t i o n V T o o l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n C o p y C a m e r a L o c a t i o n T o C u r r e n t S e l e c t i o n ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n E n g a g e C o p y C a m e r a L o c a t i o n A t t a c h ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n E s c a p e ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n R e n a m e C u r r e n t P t y ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n S e l e c t A l l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n D e s e l e c t A l l ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n T o g g l e S h o w I H M ( ) ) ;  
  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n F r e e A s p e c t R a t i o ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n 1 6 9 A s p e c t R a t i o ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n 4 3 A s p e c t R a t i o ( ) ) ;  
         t h i s - > a d d A c t i o n ( p C t r l - > g e t A c t i o n V i e w U s e r ( ) ) ;  
  
          
  
         a c t i o n K e y 1                   =   p C t r l - > g e t A c t i o n G e t M e m o C a m 1 ( ) ;  
         a c t i o n K e y 2                   =   p C t r l - > g e t A c t i o n G e t M e m o C a m 2 ( ) ;  
         a c t i o n K e y 3                   =   p C t r l - > g e t A c t i o n G e t M e m o C a m 3 ( ) ;  
         a c t i o n K e y 4                   =   p C t r l - > g e t A c t i o n G e t M e m o C a m 4 ( ) ;  
         a c t i o n K e y 5                   =   p C t r l - > g e t A c t i o n G e t M e m o C a m 5 ( ) ;  
         a c t i o n C t r l K e y 1           =   p C t r l - > g e t A c t i o n S e t M e m o C a m 1 ( ) ;  
         a c t i o n C t r l K e y 2           =   p C t r l - > g e t A c t i o n S e t M e m o C a m 2 ( ) ;  
         a c t i o n C t r l K e y 3           =   p C t r l - > g e t A c t i o n S e t M e m o C a m 3 ( ) ;  
         a c t i o n C t r l K e y 4           =   p C t r l - > g e t A c t i o n S e t M e m o C a m 4 ( ) ;  
         a c t i o n C t r l K e y 5           =   p C t r l - > g e t A c t i o n S e t M e m o C a m 5 ( ) ;  
  
  
         a c t i o n K e y P a d 1             =   p C t r l - > g e t A c t i o n V i e w T o p ( ) ;  
         a c t i o n K e y P a d 2             =   p C t r l - > g e t A c t i o n V i e w F r o n t ( ) ;  
         a c t i o n K e y P a d 3             =   p C t r l - > g e t A c t i o n V i e w R i g h t ( ) ;  
         a c t i o n C t r l K e y P a d 1     =   p C t r l - > g e t A c t i o n V i e w B o t t o m ( ) ;  
         a c t i o n C t r l K e y P a d 2     =   p C t r l - > g e t A c t i o n V i e w B a c k ( ) ;  
         a c t i o n C t r l K e y P a d 3     =   p C t r l - > g e t A c t i o n V i e w L e f t ( ) ;  
  
  
         a c t i o n K e y P a d 8             =   p C t r l - > g e t A c t i o n V i e w S o l i d ( ) ;  
         a c t i o n K e y P a d 7             =   p C t r l - > g e t A c t i o n V i e w W i r e f r a m e ( ) ;  
         a c t i o n K e y P a d 9             =   p C t r l - > g e t A c t i o n V i e w W i r e f r a m e S o l i d ( ) ;  
  
         a c t i o n K e y P a d 4             =   p C t r l - > g e t A c t i o n V i e w D e f a u l t ( ) ;  
         a c t i o n K e y P a d 5             =   p C t r l - > g e t A c t i o n V i e w N o r m a l ( ) ;  
         a c t i o n K e y P a d 6             =   p C t r l - > g e t A c t i o n V i e w U V ( ) ;  
 }  
 / / - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 A p p l i c a t i o n W i n d o w B a s e : : ~ A p p l i c a t i o n W i n d o w B a s e ( )  
 {  
 }  
 / / - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 v o i d   A p p l i c a t i o n W i n d o w B a s e : : k e y P r e s s E v e n t ( Q K e y E v e n t *   e v e n t )  
 {  
          
         i f   ( e v e n t - > m o d i f i e r s ( )   &   Q t : : K e y p a d M o d i f i e r )  
         {  
                 i f   ( e v e n t - > m o d i f i e r s ( )   &   Q t : : C o n t r o l M o d i f i e r )  
                 {  
                         s w i t c h ( e v e n t - > k e y ( ) )  
                         {  
                                 c a s e   Q t : : K e y _ 1 :   i f   ( a c t i o n K e y P a d 1 ! = N U L L )   a c t i o n C t r l K e y P a d 1 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 2 :   i f   ( a c t i o n K e y P a d 2 ! = N U L L )   a c t i o n C t r l K e y P a d 2 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 3 :   i f   ( a c t i o n K e y P a d 3 ! = N U L L )   a c t i o n C t r l K e y P a d 3 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 4 :   i f   ( a c t i o n K e y P a d 4 ! = N U L L )   a c t i o n C t r l K e y P a d 4 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 5 :   i f   ( a c t i o n K e y P a d 5 ! = N U L L )   a c t i o n C t r l K e y P a d 5 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 6 :   i f   ( a c t i o n K e y P a d 6 ! = N U L L )   a c t i o n C t r l K e y P a d 6 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 7 :   i f   ( a c t i o n K e y P a d 7 ! = N U L L )   a c t i o n C t r l K e y P a d 7 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 8 :   i f   ( a c t i o n K e y P a d 8 ! = N U L L )   a c t i o n C t r l K e y P a d 8 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 9 :   i f   ( a c t i o n K e y P a d 9 ! = N U L L )   a c t i o n C t r l K e y P a d 9 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 0 :   i f   ( a c t i o n K e y P a d 0 ! = N U L L )   a c t i o n C t r l K e y P a d 0 - > t r i g g e r ( ) ;   b r e a k ;  
                         }  
                 }  
                 e l s e  
                 {  
                         s w i t c h ( e v e n t - > k e y ( ) )  
                         {  
                                 c a s e   Q t : : K e y _ 1 :   i f   ( a c t i o n K e y P a d 1 ! = N U L L )   a c t i o n K e y P a d 1 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 2 :   i f   ( a c t i o n K e y P a d 2 ! = N U L L )   a c t i o n K e y P a d 2 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 3 :   i f   ( a c t i o n K e y P a d 3 ! = N U L L )   a c t i o n K e y P a d 3 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 4 :   i f   ( a c t i o n K e y P a d 4 ! = N U L L )   a c t i o n K e y P a d 4 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 5 :   i f   ( a c t i o n K e y P a d 5 ! = N U L L )   a c t i o n K e y P a d 5 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 6 :   i f   ( a c t i o n K e y P a d 6 ! = N U L L )   a c t i o n K e y P a d 6 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 7 :   i f   ( a c t i o n K e y P a d 7 ! = N U L L )   a c t i o n K e y P a d 7 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 8 :   i f   ( a c t i o n K e y P a d 8 ! = N U L L )   a c t i o n K e y P a d 8 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 9 :   i f   ( a c t i o n K e y P a d 9 ! = N U L L )   a c t i o n K e y P a d 9 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ 0 :   i f   ( a c t i o n K e y P a d 0 ! = N U L L )   a c t i o n K e y P a d 0 - > t r i g g e r ( ) ;   b r e a k ;  
                         }  
                 }  
         }  
         e l s e  
         {  
                 i f   ( e v e n t - > m o d i f i e r s ( )   &   Q t : : C o n t r o l M o d i f i e r )  
                 {  
                         s w i t c h ( e v e n t - > k e y ( ) )  
                         {  
                                 c a s e   Q t : : K e y _ A m p e r s a n d :           i f   ( a c t i o n K e y 1 ! = N U L L )   a c t i o n C t r l K e y 1 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e     0 x e 9 :                                   i f   ( a c t i o n K e y 2 ! = N U L L )   a c t i o n C t r l K e y 2 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ Q u o t e D b l :             i f   ( a c t i o n K e y 3 ! = N U L L )   a c t i o n C t r l K e y 3 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ A p o s t r o p h e :         i f   ( a c t i o n K e y 4 ! = N U L L )   a c t i o n C t r l K e y 4 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ P a r e n L e f t :           i f   ( a c t i o n K e y 5 ! = N U L L )   a c t i o n C t r l K e y 5 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ M i n u s :                   i f   ( a c t i o n K e y 6 ! = N U L L )   a c t i o n C t r l K e y 6 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ E g r a v e :                 i f   ( a c t i o n K e y 7 ! = N U L L )   a c t i o n C t r l K e y 7 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ U n d e r s c o r e :         i f   ( a c t i o n K e y 8 ! = N U L L )   a c t i o n C t r l K e y 8 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ C c e d i l l a :             i f   ( a c t i o n K e y 9 ! = N U L L )   a c t i o n C t r l K e y 9 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ A g r a v e :                 i f   ( a c t i o n K e y 0 ! = N U L L )   a c t i o n C t r l K e y 0 - > t r i g g e r ( ) ;   b r e a k ;  
                         }  
                 }  
                 e l s e  
                 {  
                         s w i t c h ( e v e n t - > k e y ( ) )  
                         {  
                                 c a s e   Q t : : K e y _ A m p e r s a n d :           i f   ( a c t i o n K e y 1 ! = N U L L )   a c t i o n K e y 1 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ E a c u t e :                 i f   ( a c t i o n K e y 2 ! = N U L L )   a c t i o n K e y 2 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ Q u o t e D b l :             i f   ( a c t i o n K e y 3 ! = N U L L )   a c t i o n K e y 3 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ A p o s t r o p h e :         i f   ( a c t i o n K e y 4 ! = N U L L )   a c t i o n K e y 4 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ P a r e n L e f t :           i f   ( a c t i o n K e y 5 ! = N U L L )   a c t i o n K e y 5 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ M i n u s :                   i f   ( a c t i o n K e y 6 ! = N U L L )   a c t i o n K e y 6 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ E g r a v e :                 i f   ( a c t i o n K e y 7 ! = N U L L )   a c t i o n K e y 7 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ U n d e r s c o r e :         i f   ( a c t i o n K e y 8 ! = N U L L )   a c t i o n K e y 8 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ C c e d i l l a :             i f   ( a c t i o n K e y 9 ! = N U L L )   a c t i o n K e y 9 - > t r i g g e r ( ) ;   b r e a k ;  
                                 c a s e   Q t : : K e y _ A g r a v e :                 i f   ( a c t i o n K e y 0 ! = N U L L )   a c t i o n K e y 0 - > t r i g g e r ( ) ;   b r e a k ;  
                         }  
                 }  
         }  
  
         Q M a i n W i n d o w : : k e y P r e s s E v e n t ( e v e n t ) ;  
 }  
 / / - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 v o i d   A p p l i c a t i o n W i n d o w B a s e : : k e y R e l e a s e E v e n t ( Q K e y E v e n t *   e v e n t )  
 {  
         Q M a i n W i n d o w : : k e y R e l e a s e E v e n t ( e v e n t ) ;  
 }  
 / / - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
 }   / /   n a m e s p a c e   E P I  
 